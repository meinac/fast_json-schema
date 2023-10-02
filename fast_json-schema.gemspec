# frozen_string_literal: true

require_relative "lib/fast_json/schema/version"

Gem::Specification.new do |spec|
  spec.name = "fast_json-schema"
  spec.version = FastJSON::Schema::VERSION
  spec.authors = ["Mehmet Emin INAC"]
  spec.email = ["mehmetemininac@gmail.com"]

  spec.summary = "Fastest JSON schema validation library"
  spec.description = "Validates JSON schemas against given meta-schema"
  spec.homepage = "https://github.com/meinac/fast_json-schema"
  spec.license = "MIT"
  spec.required_ruby_version = ">= 2.6.0"

  spec.metadata["homepage_uri"] = spec.homepage
  spec.metadata["source_code_uri"] = spec.homepage

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(__dir__) do
    `git ls-files -z`.split("\x0").reject do |f|
      (f == __FILE__) || f.match(%r{\A(?:(?:bin|test|spec|features)/|\.(?:git|travis|circleci)|appveyor)})
    end
  end
  spec.bindir = "exe"
  spec.executables = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions    = ["ext/fast_json/schema/extconf.rb"]
end
