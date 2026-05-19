# frozen_string_literal: true

require "json"
require "yaml"
require "set"

module Conformance
  SUITE_RELATIVE_PATH = "vendor/JSON-Schema-Test-Suite"
  DRAFT7_DIR = "tests/draft7"
  PENDING_FILE = File.expand_path("draft7_pending.yml", __dir__)

  def self.suite_root
    ENV["JSON_SCHEMA_TEST_SUITE_PATH"] ||
      File.expand_path("../../#{SUITE_RELATIVE_PATH}", __dir__)
  end

  def self.suite_available?
    File.directory?(File.join(suite_root, DRAFT7_DIR))
  end

  def self.draft7_files
    root = File.join(suite_root, DRAFT7_DIR)

    core = Dir.glob(File.join(root, "*.json")).sort
    optional = Dir.glob(File.join(root, "optional", "**", "*.json")).sort

    (core + optional).map { |path| [relative_path(path), path] }
  end

  def self.relative_path(absolute)
    base = File.join(suite_root, DRAFT7_DIR) + "/"

    absolute.sub(base, "")
  end

  def self.pending_entries_raw
    @pending_entries_raw ||= raw_pending_entries.map { |entry| enrich_entry(entry) }
  end

  def self.raw_pending_entries
    return [] unless File.exist?(PENDING_FILE)

    YAML.safe_load(File.read(PENDING_FILE)) || []
  end

  def self.enrich_entry(entry)
    entry.merge(
      "optional" => entry.fetch("optional") { entry["file"].to_s.start_with?("optional/") },
      "ignored"  => entry.fetch("ignored", false)
    )
  end

  def self.pending_entries
    @pending_entries ||= begin
      pending_entries_raw.each_with_object({}) do |entry, acc|
        key = [entry["file"], entry["group"], entry["test"]]
        acc[key] = entry["reason"] || "pending"
      end
    end
  end

  def self.pending_reason_for(file, group, test)
    pending_entries[[file, group, test]]
  end
end
