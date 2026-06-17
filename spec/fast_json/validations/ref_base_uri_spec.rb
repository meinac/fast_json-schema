# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "$ref with base URI resolution" do
    using RSpec::Parameterized::TableSyntax

    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    subject { schema.valid?(data) }

    describe "relative $ref joined against root $id (within allOf)" do
      let(:ruby_schema) do
        {
          "$id" => "http://example.com/root/",
          "allOf" => [{ "$ref" => "integer" }],
          "definitions" => {
            "S" => {
              "$id" => "http://example.com/root/integer",
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

    describe "recursive references between schemas via relative $ref" do
      let(:ruby_schema) do
        {
          "$id" => "http://localhost:1234/tree",
          "type" => "object",
          "properties" => {
            "meta" => { "type" => "string" },
            "nodes" => {
              "type" => "array",
              "items" => { "$ref" => "node" }
            }
          },
          "required" => ["meta", "nodes"],
          "definitions" => {
            "node" => {
              "$id" => "http://localhost:1234/node",
              "type" => "object",
              "properties" => {
                "value" => { "type" => "number" },
                "subtree" => { "$ref" => "tree" }
              },
              "required" => ["value"]
            }
          }
        }
      end

      where(:data, :valid?) do
        {
          "meta" => "root",
          "nodes" => [
            {
              "value" => 1,
              "subtree" => {
                "meta" => "child",
                "nodes" => [
                  {
                    "value" => 1.1
                  }
                ]
              }
            }
          ]
        } | true

        {
          "meta" => "root",
          "nodes" => [
            {
              "value" => 1,
              "subtree" => {
                "meta" => "child",
                "nodes" => [
                  {
                    "value" => "bad"
                  }
                ]
              }
            }
          ]
        } | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "relative $id propagates base for nested refs" do
      let(:ruby_schema) do
        {
          "$id" => "http://example.com/schema1.json",
          "properties" => {
            "foo" => {
              "$id" => "schema2.json",
              "definitions" => {
                "inner" => {
                  "properties" => {
                    "bar" => { "type" => "string" }
                  }
                }
              },
              "allOf" => [
                { "$ref" => "#/definitions/inner" }
              ]
            }
          },
          "allOf" => [
            { "$ref" => "schema2.json" }
          ]
        }
      end

      where(:data, :valid?) do
        { "foo" => { "bar" => "a" }, "bar" => "a" } | true
        { "foo" => { "bar" => 1 }, "bar" => "a" }   | false
        { "foo" => { "bar" => "a" }, "bar" => 1 }   | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "anchor under a non-root base" do
      let(:ruby_schema) do
        {
          "$id" => "https://example.com/schema-with-anchor",
          "allOf" => [
            { "$ref" => "https://example.com/schema-with-anchor#foo" }
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

    describe "absolute-path $ref joined against base scheme+authority" do
      let(:ruby_schema) do
        {
          "$id" => "http://example.com/ref/absref.json",
          "definitions" => {
            "a" => { "$id" => "http://example.com/ref/absref/foobar.json", "type" => "number" },
            "b" => { "$id" => "http://example.com/absref/foobar.json", "type" => "string" }
          },
          "allOf" => [
            { "$ref" => "/absref/foobar.json" }
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

    describe "nearest-parent $id wins over root" do
      let(:ruby_schema) do
        {
          "$id" => "http://example.com/a.json",
          "definitions" => {
            "x" => {
              "$id" => "http://example.com/b/c.json",
              "not" => {
                "definitions" => {
                  "y" => {
                    "$id" => "d.json",
                    "type" => "number"
                  }
                }
              }
            }
          },
          "allOf" => [
            { "$ref" => "http://example.com/b/d.json" }
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

    describe "URN base URI with URN $ref carrying a JSON Pointer fragment" do
      let(:ruby_schema) do
        {
          "$id" => "urn:uuid:deadbeef-1234-0000-0000-4321feebdaed",
          "properties" => {
            "foo" => {
              "$ref" => "urn:uuid:deadbeef-1234-0000-0000-4321feebdaed#/definitions/bar"
            }
          },
          "definitions" => {
            "bar" => { "type" => "string" }
          }
        }
      end

      where(:data, :valid?) do
        { "foo" => "x" } | true
        { "foo" => 1 }   | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "unresolved relative $ref raises with resolved form" do
      let(:ruby_schema) do
        {
          "$id" => "http://example.com/root/",
          "allOf" => [
            { "$ref" => "missing" }
          ]
        }
      end

      it "raises a RuntimeError mentioning the resolved form" do
        expect { schema }.to raise_error(
          RuntimeError,
          %r{Unresolved \$ref: missing \(resolved to http://example.com/root/missing\)}
        )
      end
    end
  end
end
