# frozen_string_literal: true

require "rspec/core"
require "rspec/core/formatters/json_formatter"
require "stringio"

require_relative "../conformance"

module Conformance
  module Reporter
    DRAFT7_SPEC_FILE = File.expand_path("../draft7_spec.rb", __dir__)

    README_START_MARKER = "<!-- conformance:start -->"
    README_END_MARKER = "<!-- conformance:end -->"

    def self.run_and_collect_stats
      json_output = StringIO.new
      formatter = RSpec::Core::Formatters::JsonFormatter.new(json_output)

      options = RSpec::Core::ConfigurationOptions.new([DRAFT7_SPEC_FILE])
      runner = RSpec::Core::Runner.new(options)

      config = runner.instance_variable_get(:@configuration) || RSpec.configuration
      config.reset_reporter if config.respond_to?(:reset_reporter)
      config.add_formatter(formatter)

      exit_code = runner.run($stderr, StringIO.new)

      result = formatter.output_hash || {}
      summary = result[:summary] || {}

      total = summary[:example_count].to_i
      failed = summary[:failure_count].to_i
      pending = summary[:pending_count].to_i
      passed = total - failed - pending

      { total: total, passed: passed, pending: pending, failed: failed, exit_code: exit_code }
    end

    def self.formatted_gaps
      entries = Conformance.pending_entries_raw

      return "No pending Draft-7 conformance entries." if entries.size.zero?

      lines = ["Draft-7 conformance pending (#{gaps[:total]} total):"]

      sorted = entries.group_by { |e| e["category"] || "uncategorized" }
                      .sort_by { |category, entries| [-entries.size, category.to_s] }

      sorted.each do |category, entries|
        lines << "  #{category}: #{entries.size}"

        example_reasons = entries.map { |e| e["reason"] }.compact.uniq.first(3)
        example_reasons.each { |reason| lines << "    - #{reason}" }
      end

      lines.join("\n")
    end

    def self.format_stats(summary)
      lines = [
        "Draft-7 conformance:",
        "  total:    #{summary[:total]}",
        "  passed:   #{summary[:passed]}",
        "  pending:  #{summary[:pending]}",
        "  failed:   #{summary[:failed]}"
      ]

      pct = (summary[:passed].to_f / summary[:total].to_f * 100).round(1)
      lines << "  pass rate: #{pct}% (#{summary[:passed]}/#{summary[:total]})"

      lines.join("\n")
    end

    def self.update_readme!(readme_path, summary)
      content = File.read(readme_path)

      pct = (summary[:passed].to_f / summary[:total].to_f * 100).round(1)

      block = <<~MD.chomp
        #{README_START_MARKER}
        **Draft-7**: #{summary[:passed]} / #{summary[:total]} (#{pct}%) — #{summary[:pending]} pending, #{summary[:failed]} failing.

        Remote-ref resolution (`refRemote.json`) is intentionally not implemented and contributes to the failing count.
        #{README_END_MARKER}
      MD

      pattern = /#{Regexp.escape(README_START_MARKER)}.*?#{Regexp.escape(README_END_MARKER)}/m

      if content.match?(pattern)
        content = content.sub(pattern, block)
      else
        raise "README conformance markers not found; expected #{README_START_MARKER} ... #{README_END_MARKER}"
      end

      File.write(readme_path, content)
    end
  end
end
