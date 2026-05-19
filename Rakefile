# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

RSpec::Core::RakeTask.new(:all_specs) do |t|
  t.pattern = "spec/**/*_spec.rb"
  t.exclude_pattern = "spec/conformance/**/*_spec.rb"
end

RSpec::Core::RakeTask.new(:spec) do |t|
  t.exclude_pattern = "spec/gc_stress/**/*_spec.rb,spec/conformance/**/*_spec.rb"
end

namespace :spec do
  RSpec::Core::RakeTask.new(:gc_stress) do |t|
    t.pattern = "spec/gc_stress/**/*_spec.rb"
  end
end

Rake::ExtensionTask.new "schema" do |ext|
  ext.lib_dir = "lib/fast_json/schema/ext"
  ext.ext_dir = 'ext/fast_json/schema'
end

namespace :conformance do
  desc "Run the official JSON Schema Test Suite for Draft 7 (core + optional)"
  RSpec::Core::RakeTask.new(:draft7) do |t|
    t.pattern = "spec/conformance/draft7_spec.rb"
  end

  desc "Print a category summary of pending Draft-7 conformance cases"
  task "draft7:gaps" do
    require_relative "spec/conformance/conformance"
    require_relative "spec/conformance/conformance/reporter"

    puts Conformance::Reporter.formatted_gaps
  end

  desc "Run Draft-7 conformance suite in-process and print pass/pending/fail counts"
  task "draft7:stats" do
    require_relative "spec/conformance/conformance"
    require_relative "spec/conformance/conformance/reporter"

    summary = Conformance::Reporter.run_and_collect_stats

    puts Conformance::Reporter.format_stats(summary)

    if ENV["UPDATE_README"] == "1"
      readme_path = File.expand_path("README.md", __dir__)
      Conformance::Reporter.update_readme!(readme_path, summary)

      puts "Updated #{readme_path}"
    end
  end
end

task all: [:compile, :all_specs]
task default: [:compile, :spec]