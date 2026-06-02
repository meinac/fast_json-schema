# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "$ref" do
    using RSpec::Parameterized::TableSyntax

    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    subject { schema.valid?(data) }

    describe "local ref to a sibling property" do
      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "name"     => { "type" => "string", "minLength" => 1 },
            "nickname" => { "$ref" => "#/properties/name" }
          }
        }
      end

      where(:data, :valid?) do
        { "name" => "alice", "nickname" => "ally" } | true
        { "name" => "alice", "nickname" => "" }     | false
        { "name" => "alice", "nickname" => 5 }      | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref ignores sibling keywords (Draft 7)" do
      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "name"  => { "type" => "string", "minLength" => 1 },
            "alias" => { "$ref" => "#/properties/name", "type" => "integer" }
          }
        }
      end

      where(:data, :valid?) do
        { "name" => "a", "alias" => "b" } | true
        { "name" => "a", "alias" => 5 }   | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "recursive self-ref" do
      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "value" => { "type" => "integer" },
            "next"  => { "$ref" => "#" }
          },
          "required" => ["value"]
        }
      end

      where(:data, :valid?) do
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => 3 } } }     | true
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => "bad" } } } | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into #/definitions" do
      let(:ruby_schema) do
        {
          "$ref" => "#/definitions/Address",
          "definitions" => {
            "Address" => { "type" => "string", "minLength" => 1 }
          }
        }
      end

      where(:data, :valid?) do
        "home" | true
        ""     | false
        5      | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into #/$defs" do
      let(:ruby_schema) do
        {
          "$ref" => "#/$defs/Email",
          "$defs" => {
            "Email" => { "type" => "string", "minLength" => 3 }
          }
        }
      end

      where(:data, :valid?) do
        "ab@"  | true
        "a"    | false
        42     | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into nested custom container" do
      let(:ruby_schema) do
        {
          "$ref" => "#/components/schemas/User",
          "components" => {
            "schemas" => {
              "User" => {
                "type" => "object",
                "required" => ["id"],
                "properties" => {
                  "id" => { "type" => "integer" }
                }
              }
            }
          }
        }
      end

      where(:data, :valid?) do
        { "id" => 1 }   | true
        { "id" => "x" } | false
        {}              | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into known keyword on the same node" do
      let(:ruby_schema) do
        {
          "$ref" => "#/properties/name",
          "properties" => {
            "name" => { "type" => "string", "minLength" => 1 }
          }
        }
      end

      where(:data, :valid?) do
        "alice" | true
        ""      | false
        5       | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "cross-reference between definitions" do
      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "value" => { "$ref" => "#/definitions/A" }
          },
          "definitions" => {
            "A" => { "$ref" => "#/definitions/B" },
            "B" => { "type" => "integer" }
          }
        }
      end

      where(:data, :valid?) do
        { "value" => 1 }     | true
        { "value" => "bad" } | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "recursive self-reference inside a definition" do
      let(:ruby_schema) do
        {
          "$ref" => "#/definitions/Node",
          "definitions" => {
            "Node" => {
              "type" => "object",
              "properties" => {
                "value" => { "type" => "integer" },
                "next"  => { "$ref" => "#/definitions/Node" }
              },
              "required" => ["value"]
            }
          }
        }
      end

      where(:data, :valid?) do
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => 3 } } }     | true
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => "bad" } } } | false
        { "next" => { "value" => 1 } }                                               | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "unresolved $ref into a missing definition" do
      let(:ruby_schema) do
        {
          "$ref" => "#/definitions/Missing",
          "definitions" => {
            "Other" => { "type" => "string" }
          }
        }
      end

      it "raises a RuntimeError when compiled" do
        expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
      end
    end

    describe "$ref with JSON Pointer escape sequences and percent-encoded characters" do
      let(:ruby_schema) do
        {
          "definitions" => {
            "tilde~field"   => { "type" => "integer" },
            "slash/field"   => { "type" => "integer" },
            "percent%field" => { "type" => "integer" },
            "~0"            => { "type" => "integer" }
          },
          "properties" => {
            "tilde"        => { "$ref" => "#/definitions/tilde~0field" },
            "slash"        => { "$ref" => "#/definitions/slash~1field" },
            "percent"      => { "$ref" => "#/definitions/percent%25field" },
            "literal_til0" => { "$ref" => "#/definitions/~00" }
          }
        }
      end

      describe "valid?" do
        where(:data, :valid?) do
          { "tilde"        => 1 }     | true
          { "slash"        => 1 }     | true
          { "percent"      => 1 }     | true
          { "literal_til0" => 1 }     | true
          { "tilde"        => "foo" } | false
          { "slash"        => "foo" } | false
          { "percent"      => "foo" } | false
          { "literal_til0" => "foo" } | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "unresolved escaped $ref" do
        let(:ruby_schema) do
          { "$ref" => "#/definitions/missing~0name" }
        end

        it "raises RuntimeError with the raw (un-decoded) $ref in the message" do
          expect { schema }.to raise_error(RuntimeError, %r{Unresolved \$ref: \#/definitions/missing~0name})
        end
      end
    end

    describe "$ref into all known keywords under #/definitions" do
      let(:ruby_schema) do
        {
          "definitions" => {
            "kw_type"                => { "type" => "integer" },
            "kw_enum"                => { "enum" => [1, 2, 3] },
            "kw_const"               => { "const" => "fixed" },

            "kw_multipleOf"          => { "type" => "integer", "multipleOf" => 3 },
            "kw_maximum"             => { "type" => "integer", "maximum" => 10 },
            "kw_exclusiveMaximum"    => { "type" => "integer", "exclusiveMaximum" => 10 },
            "kw_minimum"             => { "type" => "integer", "minimum" => 0 },
            "kw_exclusiveMinimum"    => { "type" => "integer", "exclusiveMinimum" => 0 },

            "kw_maxLength"           => { "type" => "string", "maxLength" => 3 },
            "kw_minLength"           => { "type" => "string", "minLength" => 2 },
            "kw_pattern"             => { "type" => "string", "pattern" => "^[a-z]+$" },

            "kw_items_schema"        => { "type" => "array", "items" => { "type" => "integer" } },
            "kw_items_array"         => { "type" => "array", "items" => [{ "type" => "integer" }, { "type" => "string" }] },
            "kw_additionalItems"     => { "type" => "array", "items" => [{ "type" => "integer" }], "additionalItems" => { "type" => "string" } },
            "kw_contains"            => { "type" => "array", "contains" => { "type" => "integer" } },
            "kw_maxItems"            => { "type" => "array", "maxItems" => 2 },
            "kw_minItems"            => { "type" => "array", "minItems" => 2 },
            "kw_uniqueItems"         => { "type" => "array", "uniqueItems" => true },
            "kw_maxContains"         => { "type" => "array", "contains" => { "type" => "integer" }, "maxContains" => 2 },
            "kw_minContains"         => { "type" => "array", "contains" => { "type" => "integer" }, "minContains" => 2 },

            "kw_properties"          => { "type" => "object", "properties" => { "x" => { "type" => "integer" } } },
            "kw_patternProperties"   => { "type" => "object", "patternProperties" => { "^x" => { "type" => "integer" } } },
            "kw_additionalProperties" => { "type" => "object", "additionalProperties" => { "type" => "integer" } },
            "kw_propertyNames"       => { "type" => "object", "propertyNames" => { "minLength" => 2 } },
            "kw_maxProperties"       => { "type" => "object", "maxProperties" => 2 },
            "kw_minProperties"       => { "type" => "object", "minProperties" => 1 },
            "kw_required"            => { "type" => "object", "required" => ["x"] },
            "kw_dependencies_array"  => { "type" => "object", "dependencies" => { "x" => ["y"] } },
            "kw_dependencies_schema" => { "type" => "object", "dependencies" => { "x" => { "required" => ["y"] } } },

            "kw_allOf"               => { "allOf" => [{ "type" => "integer" }, { "minimum" => 1 }] },
            "kw_anyOf"               => { "anyOf" => [{ "type" => "integer" }, { "type" => "string" }] },
            "kw_oneOf"               => { "oneOf" => [{ "type" => "integer" }, { "type" => "string" }] },
            "kw_not"                 => { "not" => { "type" => "string" } },

            "kw_if_then_else"        => {
              "if"   => { "type" => "integer" },
              "then" => { "minimum" => 0 },
              "else" => { "type" => "string" }
            },

            "kw_format"              => { "type" => "string", "format" => "date" }
          },
          "properties" => {
            "kw_type_p"                 => { "$ref" => "#/definitions/kw_type" },
            "kw_enum_p"                 => { "$ref" => "#/definitions/kw_enum" },
            "kw_const_p"                => { "$ref" => "#/definitions/kw_const" },

            "kw_multipleOf_p"           => { "$ref" => "#/definitions/kw_multipleOf" },
            "kw_maximum_p"              => { "$ref" => "#/definitions/kw_maximum" },
            "kw_exclusiveMaximum_p"     => { "$ref" => "#/definitions/kw_exclusiveMaximum" },
            "kw_minimum_p"              => { "$ref" => "#/definitions/kw_minimum" },
            "kw_exclusiveMinimum_p"     => { "$ref" => "#/definitions/kw_exclusiveMinimum" },

            "kw_maxLength_p"            => { "$ref" => "#/definitions/kw_maxLength" },
            "kw_minLength_p"            => { "$ref" => "#/definitions/kw_minLength" },
            "kw_pattern_p"              => { "$ref" => "#/definitions/kw_pattern" },

            "kw_items_schema_p"         => { "$ref" => "#/definitions/kw_items_schema" },
            "kw_items_array_p"          => { "$ref" => "#/definitions/kw_items_array" },
            "kw_additionalItems_p"      => { "$ref" => "#/definitions/kw_additionalItems" },
            "kw_contains_p"             => { "$ref" => "#/definitions/kw_contains" },
            "kw_maxItems_p"             => { "$ref" => "#/definitions/kw_maxItems" },
            "kw_minItems_p"             => { "$ref" => "#/definitions/kw_minItems" },
            "kw_uniqueItems_p"          => { "$ref" => "#/definitions/kw_uniqueItems" },
            "kw_maxContains_p"          => { "$ref" => "#/definitions/kw_maxContains" },
            "kw_minContains_p"          => { "$ref" => "#/definitions/kw_minContains" },

            "kw_properties_p"           => { "$ref" => "#/definitions/kw_properties" },
            "kw_patternProperties_p"    => { "$ref" => "#/definitions/kw_patternProperties" },
            "kw_additionalProperties_p" => { "$ref" => "#/definitions/kw_additionalProperties" },
            "kw_propertyNames_p"        => { "$ref" => "#/definitions/kw_propertyNames" },
            "kw_maxProperties_p"        => { "$ref" => "#/definitions/kw_maxProperties" },
            "kw_minProperties_p"        => { "$ref" => "#/definitions/kw_minProperties" },
            "kw_required_p"             => { "$ref" => "#/definitions/kw_required" },
            "kw_dependencies_array_p"   => { "$ref" => "#/definitions/kw_dependencies_array" },
            "kw_dependencies_schema_p"  => { "$ref" => "#/definitions/kw_dependencies_schema" },

            "kw_allOf_p"                => { "$ref" => "#/definitions/kw_allOf" },
            "kw_anyOf_p"                => { "$ref" => "#/definitions/kw_anyOf" },
            "kw_oneOf_p"                => { "$ref" => "#/definitions/kw_oneOf" },
            "kw_not_p"                  => { "$ref" => "#/definitions/kw_not" },

            "kw_if_then_else_p"         => { "$ref" => "#/definitions/kw_if_then_else" },

            "kw_format_p"               => { "$ref" => "#/definitions/kw_format" }
          }
        }
      end

      where(:data, :valid?) do
        { "kw_type_p" => 1 }                                         | true
        { "kw_type_p" => "x" }                                       | false
        { "kw_enum_p" => 2 }                                         | true
        { "kw_enum_p" => 4 }                                         | false
        { "kw_const_p" => "fixed" }                                  | true
        { "kw_const_p" => "other" }                                  | false

        { "kw_multipleOf_p" => 9 }                                   | true
        { "kw_multipleOf_p" => 10 }                                  | false
        { "kw_maximum_p" => 10 }                                     | true
        { "kw_maximum_p" => 11 }                                     | false
        { "kw_exclusiveMaximum_p" => 9 }                             | true
        { "kw_exclusiveMaximum_p" => 10 }                            | false
        { "kw_minimum_p" => 0 }                                      | true
        { "kw_minimum_p" => -1 }                                     | false
        { "kw_exclusiveMinimum_p" => 1 }                             | true
        { "kw_exclusiveMinimum_p" => 0 }                             | false

        { "kw_maxLength_p" => "abc" }                                | true
        { "kw_maxLength_p" => "abcd" }                               | false
        { "kw_minLength_p" => "ab" }                                 | true
        { "kw_minLength_p" => "a" }                                  | false
        { "kw_pattern_p" => "abc" }                                  | true
        { "kw_pattern_p" => "AB1" }                                  | false

        { "kw_items_schema_p" => [1, 2] }                            | true
        { "kw_items_schema_p" => [1, "x"] }                          | false
        { "kw_items_array_p" => [1, "x"] }                           | true
        { "kw_items_array_p" => [1, 2] }                             | false
        { "kw_additionalItems_p" => [1, "x", "y"] }                  | true
        { "kw_additionalItems_p" => [1, "x", 2] }                    | false
        { "kw_contains_p" => ["x", 1, "y"] }                         | true
        { "kw_contains_p" => ["x", "y"] }                            | false
        { "kw_maxItems_p" => [1, 2] }                                | true
        { "kw_maxItems_p" => [1, 2, 3] }                             | false
        { "kw_minItems_p" => [1, 2] }                                | true
        { "kw_minItems_p" => [1] }                                   | false
        { "kw_uniqueItems_p" => [1, 2, 3] }                          | true
        { "kw_uniqueItems_p" => [1, 1] }                             | false
        { "kw_maxContains_p" => [1, 2, "x"] }                        | true
        { "kw_maxContains_p" => [1, 2, 3] }                          | false
        { "kw_minContains_p" => [1, 2] }                             | true
        { "kw_minContains_p" => [1] }                                | false

        { "kw_properties_p" => { "x" => 1 } }                        | true
        { "kw_properties_p" => { "x" => "y" } }                      | false
        { "kw_patternProperties_p" => { "x1" => 1 } }                | true
        { "kw_patternProperties_p" => { "x1" => "y" } }              | false
        { "kw_additionalProperties_p" => { "a" => 1 } }              | true
        { "kw_additionalProperties_p" => { "a" => "y" } }            | false
        { "kw_propertyNames_p" => { "abc" => 1 } }                   | true
        { "kw_propertyNames_p" => { "a" => 1 } }                     | false
        { "kw_maxProperties_p" => { "a" => 1 } }                     | true
        { "kw_maxProperties_p" => { "a" => 1, "b" => 2, "c" => 3 } } | false
        { "kw_minProperties_p" => { "a" => 1 } }                     | true
        { "kw_minProperties_p" => {} }                               | false
        { "kw_required_p" => { "x" => 1 } }                          | true
        { "kw_required_p" => { "y" => 1 } }                          | false
        { "kw_dependencies_array_p" => { "x" => 1, "y" => 2 } }      | true
        { "kw_dependencies_array_p" => { "x" => 1 } }                | false
        { "kw_dependencies_schema_p" => { "x" => 1, "y" => 2 } }     | true
        { "kw_dependencies_schema_p" => { "x" => 1 } }               | false

        { "kw_allOf_p" => 2 }                                        | true
        { "kw_allOf_p" => "x" }                                      | false
        { "kw_anyOf_p" => "x" }                                      | true
        { "kw_anyOf_p" => [1] }                                      | false
        { "kw_oneOf_p" => "x" }                                      | true
        { "kw_oneOf_p" => [1] }                                      | false
        { "kw_not_p" => 1 }                                          | true
        { "kw_not_p" => "x" }                                        | false

        { "kw_if_then_else_p" => 1 }                                 | true
        { "kw_if_then_else_p" => -1 }                                | false

        { "kw_format_p" => "2020-01-01" }                            | true
        { "kw_format_p" => "not-a-date" }                            | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into a schema under an unknown keyword" do
      describe "unknown keyword whose value is a schema" do
        let(:ruby_schema) do
          {
            "$ref" => "#/customSchema",
            "customSchema" => { "type" => "integer" }
          }
        end

        where(:data, :valid?) do
          1     | true
          "foo" | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "unknown keyword whose value is an array of schemas" do
        let(:ruby_schema) do
          {
            "$ref" => "#/customList/0",
            "customList" => [
              { "type" => "integer" },
              { "type" => "string" }
            ]
          }
        end

        where(:data, :valid?) do
          1     | true
          "foo" | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref to a non-first entry in an array under an unknown keyword" do
        let(:ruby_schema) do
          {
            "$ref" => "#/customList/1",
            "customList" => [
              { "type" => "integer" },
              { "type" => "string" }
            ]
          }
        end

        where(:data, :valid?) do
          "foo" | true
          1     | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref to a true boolean schema entry in an array under an unknown keyword" do
        let(:ruby_schema) do
          {
            "$ref" => "#/customList/0",
            "customList" => [true]
          }
        end

        where(:data, :valid?) do
          1     | true
          "foo" | true
          nil   | true
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref to a false boolean schema entry in an array under an unknown keyword" do
        let(:ruby_schema) do
          {
            "$ref" => "#/customList/0",
            "customList" => [false]
          }
        end

        where(:data, :valid?) do
          1     | false
          "foo" | false
          nil   | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "nested unknown keyword whose deeper value is an array of schemas" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/inner/0",
            "outer" => {
              "inner" => [
                { "type" => "integer" }
              ]
            }
          }
        end

        where(:data, :valid?) do
          1     | true
          "foo" | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref to the array itself is unresolved" do
        let(:ruby_schema) do
          {
            "$ref" => "#/customList",
            "customList" => [{ "type" => "integer" }]
          }
        end

        it "raises a RuntimeError when compiled" do
          expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
        end
      end

      describe "$ref into a deeply nested array under an unknown keyword" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/0/0",
            "outer" => [
              [
                { "type" => "integer" }
              ]
            ]
          }
        end

        where(:data, :valid?) do
          1     | true
          "foo" | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref into an even more deeply nested array under an unknown keyword" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/0/0/0",
            "outer" => [
              [
                [
                  { "type" => "integer" }
                ]
              ]
            ]
          }
        end

        where(:data, :valid?) do
          1     | true
          "foo" | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref selects among boolean schemas inside a nested array" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/0/1",
            "outer" => [
              [true, false]
            ]
          }
        end

        where(:data, :valid?) do
          1     | false
          "foo" | false
          nil   | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref reaches both a schema and a nested-array sibling in a mixed array" do
        let(:ruby_schema) do
          {
            "type" => "object",
            "properties" => {
              "schema_ref" => { "$ref" => "#/outer/0" },
              "nested_array_ref" => { "$ref" => "#/outer/1/0" }
            },
            "outer" => [
              { "type" => "integer" },
              [{ "type" => "string" }]
            ]
          }
        end

        where(:data, :valid?) do
          { "schema_ref" => 1, "nested_array_ref" => "foo" }     | true
          { "schema_ref" => "bad", "nested_array_ref" => "foo" } | false
          { "schema_ref" => 1, "nested_array_ref" => 1 }         | false
        end

        with_them do
          it { is_expected.to be(valid?) }
        end
      end

      describe "$ref past a scalar inside a nested array is unresolved" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/0/0",
            "outer" => [
              ["not a schema"]
            ]
          }
        end

        it "raises a RuntimeError when compiled" do
          expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
        end
      end

      describe "$ref to an out-of-range index inside a nested array is unresolved" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/0/5",
            "outer" => [
              [{ "type" => "integer" }]
            ]
          }
        end

        it "raises a RuntimeError when compiled" do
          expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
        end
      end

      describe "$ref to an intermediate array (not a schema) is unresolved" do
        let(:ruby_schema) do
          {
            "$ref" => "#/outer/0",
            "outer" => [
              [{ "type" => "integer" }]
            ]
          }
        end

        it "raises a RuntimeError when compiled" do
          expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
        end
      end
    end

    describe "$ref resolves via $id on a sibling schema" do
      let(:ruby_schema) do
        {
          "$ref" => "http://example.com/integer",
          "definitions" => {
            "IntegerSchema" => {
              "$id" => "http://example.com/integer",
              "type" => "integer"
            }
          }
        }
      end

      where(:data, :valid?) do
        1     | true
        "foo" | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref resolves via $id on an if subschema" do
      let(:ruby_schema) do
        {
          "allOf" => [
            { "$ref" => "http://example.com/ref/if" },
            {
              "if" => {
                "$id" => "http://example.com/ref/if",
                "type" => "integer"
              }
            }
          ]
        }
      end

      where(:data, :valid?) do
        12    | true
        "foo" | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref resolves via $id on a then subschema" do
      let(:ruby_schema) do
        {
          "allOf" => [
            { "$ref" => "http://example.com/ref/then" },
            {
              "then" => {
                "$id" => "http://example.com/ref/then",
                "type" => "integer"
              }
            }
          ]
        }
      end

      where(:data, :valid?) do
        12    | true
        "foo" | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref resolves via $id on an else subschema" do
      let(:ruby_schema) do
        {
          "allOf" => [
            { "$ref" => "http://example.com/ref/else" },
            {
              "else" => {
                "$id" => "http://example.com/ref/else",
                "type" => "integer"
              }
            }
          ]
        }
      end

      where(:data, :valid?) do
        12    | true
        "foo" | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref resolves via $id inside an allOf entry" do
      let(:ruby_schema) do
        {
          "allOf" => [
            { "$ref" => "http://example.com/aliased" },
            {
              "$id" => "http://example.com/aliased",
              "type" => "string"
            }
          ]
        }
      end

      where(:data, :valid?) do
        "foo" | true
        1     | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref resolves via anchor-style $id" do
      let(:ruby_schema) do
        {
          "allOf" => [
            { "$ref" => "#foo" }
          ],
          "definitions" => {
            "A" => {
              "$id" => "#foo",
              "type" => "integer"
            }
          }
        }
      end

      where(:data, :valid?) do
        1     | true
        "foo" | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref to a non-existent $id remains unresolved" do
      let(:ruby_schema) do
        {
          "$ref" => "http://example.com/missing",
          "definitions" => {
            "Other" => {
              "$id" => "http://example.com/other",
              "type" => "integer"
            }
          }
        }
      end

      it "raises a RuntimeError when compiled" do
        expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
      end
    end
  end
end
