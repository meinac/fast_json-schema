# frozen_string_literal: true

require "fast_json/schema"
require_relative "conformance"

RSpec.describe "JSON Schema Test Suite (Draft 7)" do
  unless Conformance.suite_available?
    it "is unavailable (submodule not initialized)" do
      skip "Initialize the submodule: git submodule update --init --recursive"
    end

    next
  end

  Conformance.draft7_files.each do |relative_file, absolute_file|
    describe relative_file do
      groups = JSON.parse(File.read(absolute_file))

      groups.each do |group|
        group_description = group["description"]
        group_schema = group["schema"]

        describe group_description do
          group["tests"].each do |test|
            test_description = test["description"]
            test_data = test["data"]
            expected_valid = test["valid"]

            it test_description do
              reason = Conformance.pending_reason_for(relative_file, group_description, test_description)
              pending(reason) if reason

              schema = FastJSON::Schema.create(group_schema)
              is_valid = schema.valid?(test_data)

              expect(is_valid).to be(expected_valid)
            end
          end
        end
      end
    end
  end
end
