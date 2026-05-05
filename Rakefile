# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

RSpec::Core::RakeTask.new(:all_specs) do |t|
  t.pattern = "spec/**/*_spec.rb"
end

RSpec::Core::RakeTask.new(:spec) do |t|
  t.exclude_pattern = "spec/gc_stress/**/*_spec.rb"
end

namespace :spec do
  RSpec::Core::RakeTask.new(:gc_stress) do |t|
    t.pattern = "spec/gc_stress/**/*_spec.rb"
  end
end

task all: [:compile, :spec, :all_specs]
task default: [:compile, :spec]

Rake::ExtensionTask.new "schema" do |ext|
  ext.lib_dir = "lib/fast_json/schema/ext"
  ext.ext_dir = 'ext/fast_json/schema'
end
