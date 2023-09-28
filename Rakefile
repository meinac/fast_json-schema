# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

RSpec::Core::RakeTask.new(:spec)

task default: [:compile, :spec]

Rake::ExtensionTask.new "schema" do |ext|
  ext.lib_dir = "lib/fast_json/schema/ext"
  ext.ext_dir = 'ext/fast_json/schema'
end
