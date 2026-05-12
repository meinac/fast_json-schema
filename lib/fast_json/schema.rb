# frozen_string_literal: true

require_relative "schema/version"
require_relative "schema/error"
require_relative "schema/ext/schema"

class FastJSON
  class Schema
    class << self
      def create(ruby_schema, formats: nil)
        new(ruby_schema, formats: formats).compile
      end
    end

    def initialize(ruby_schema, formats: nil)
      @ruby_schema = ruby_schema
      @custom_formats = validate_custom_formats!(formats)
    end

    def invalid?(data)
      !valid?(data)
    end

    private

    def validate_custom_formats!(formats)
      return if formats.nil?

      unless formats.is_a?(Hash)
        raise TypeError, "formats must be a Hash, got #{formats.class}"
      end

      validate_each_custom_format!(formats)

      formats
    end

    def validate_each_custom_format!(formats)
      formats.each do |name, callable|
        unless name.is_a?(String)
          raise TypeError, "format name must be a String, got #{name.class}"
        end

        unless callable.respond_to?(:call)
          raise TypeError, "format validator for #{name.inspect} must respond to :call"
        end
      end
    end
  end
end
