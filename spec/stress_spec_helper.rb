require "spec_helper"

RSpec.configure do |config|
  config.around(:each, :gc_stress) do |example|
    previous = GC.stress
    GC.stress = true

    example.run
  ensure
    GC.stress = previous
  end

  config.around(:each, :gc_auto_compact) do |example|
    previous = GC.auto_compact
    GC.auto_compact = true

    example.run
  ensure
    GC.auto_compact = previous
  end
end

def force_compaction
  if GC.respond_to?(:verify_compaction_references)
    begin
      GC.verify_compaction_references(toward: :empty, expand_heap: true)
    rescue ArgumentError
      GC.verify_compaction_references
    end
  else
    GC.compact
  end
end