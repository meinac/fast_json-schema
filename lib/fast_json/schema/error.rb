# frozen_string_literal: true

class FastJSON
  class Schema
    class Error
      def initialize(schema_path, data, data_path, type)
        @schema_path = schema_path
        @data = data
        @data_path = data_path
        @type = type
      end
    end
  end
end
