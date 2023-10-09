# frozen_string_literal: true

require_relative "schema/version"
require_relative "schema/error"
require_relative "schema/ext/schema"

class FastJSON
  class Schema
    class << self
      def create(ruby_schema)
        new(ruby_schema).compile
      end
    end

    def initialize(ruby_schema)
      @ruby_schema = ruby_schema
    end

    def valid?(data)
      validate(data).none?
    end

    def invalid?(data)
      validate(data).any?
    end
  end
end
