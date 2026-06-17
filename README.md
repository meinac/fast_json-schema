# FastJson::Schema

The fastest JSON Schema validator for Ruby, implemented as a native C extension. Verified against the official Draft-7 test suite. See [Conformance](#conformance) and [Benchmarks](#benchmarks) for details.

## Installation

Install the gem and add to the application's Gemfile by executing:

    $ bundle add fast_json-schema

If bundler is not being used to manage dependencies, install the gem by executing:

    $ gem install fast_json-schema

## Usage

```ruby
require "fast_json/schema"

schema = FastJSON::Schema.create(
  "type" => "object",
  "required" => ["name"],
  "properties" => {
    "name" => {
      "type" => "string",
      "minLength" => 1
    },
    "age"  => {
      "type" => "integer",
      "minimum" => 0
    }
  }
)

schema.valid?("name" => "alice", "age" => 30) # => true
schema.valid?("name" => "")                   # => false (minLength)
schema.valid?("age"  => 30)                   # => false (required: name)

# Collect validation errors
schema.validate do |error|
  puts error
end
```

## Roadmap

This gem is under development at the moment and I am planning to support only the Draft 7 in the first version. That being said, some of the validation keywords of Draft 8 are implemented. Here is the list of keywords/annotations and the progress;

- **Annotations**
    - `$id` &check;
    - `$ref` &check;
    - `$recursiveAnchor` &cross; (Draft 8)
    - `$recursiveRef` &cross; (Draft 8)
- **Keywords for Applying Subschemas Conditionally**
    - `if` &check;
    - `then` &check;
    - `else` &check;
- **Keywords for Applying Subschemas With Boolean Logic**
    - `allOf` &check;
    - `anyOf` &check;
    - `oneOf` &check;
    - `not` &check;
- **Validation keywords for any instance type**
    - `type` &check;
    - `enum` &check;
    - `const` &check;
- **Validation Keywords for Numeric Instances (number and integer)**
    - `multipleOf` &check;
    - `maximum` &check;
    - `exclusiveMaximum` &check;
    - `minimum` &check;
    - `exclusiveMinimum` &check;
- **Validation Keywords for Strings**
    - `maxLength` &check;
    - `minLength` &check;
    - `pattern` &check;
- **Validation Keywords for Arrays**
    - `items` &check;
    - `additionalItems` &check;
    - `unevaluatedItems` &cross; (Draft 8)
    - `maxItems` &check;
    - `minItems` &check;
    - `uniqueItems` &check;
    - `contains` &check;
    - `maxContains` &check; (Draft 8)
    - `minContains` &check; (Draft 8)
- **Validation Keywords for Objects**
    - `properties` &check;
    - `patternProperties` &check;
    - `additionalProperties` &check;
    - `unevaluatedProperties` &cross; (Draft 8)
    - `propertyNames` &check;
    - `maxProperties` &check;
    - `minProperties` &check;
    - `required` &check;
    - `dependentRequired` &cross; (Draft 8)
    - `dependencies` &check;
- **Formats**
    - Enabling/disabling the `format` as assertion &cross;
    - `Custom format attributes` &check;
    - Dates, Times, and Duration
        - `date-time` &check;
        - `date` &check;
        - `time` &check;
        - `duration` &cross; (Draft 8)
    - Email Addresses
        - `email` &check;
        - `idn-email` &check;
    - Hostnames
        - `hostname` &check;
        - `idn-hostname` &check;
    - IP Addresses
        - `IPv4` &check;
        - `IPv6` &check;
    - Resource Identifiers
        - `uri` &check;
        - `uri-reference` &check;
        - `iri` &check;
        - `iri-reference` &check;
        - `uuid` &cross; (Draft 8)
    - `uri-template` &check;
    - JSON Pointers
        - `json-pointer` &check;
        - `relative-json-pointer` &check;
    - `regex` &check;
        > Compiled with Ruby's `Regexp.new` (Onigmo engine). This matches `json_schemer` / `json-schema` behavior but differs slightly from strict ECMA-262 — Onigmo accepts some constructs ECMA rejects (possessive quantifiers, atomic groups, `\A` / `\z` / `\Z` anchors) and vice versa. For strict ECMA-262 conformance, pre-process patterns with a translation library before validation.
- **Vocabulary for the Contents of String-Encoded Data**
    - I'm not planning to support validating string instances with these annotations in the near future.

## Conformance

This project is verified against the official [JSON Schema Test Suite](https://github.com/json-schema-org/JSON-Schema-Test-Suite), vendored as a git submodule at `vendor/JSON-Schema-Test-Suite`.

After cloning the repository, initialize the submodule:

```sh
git submodule update --init --recursive
```

Then run the Draft-7 conformance suite (includes core + optional tests):

```sh
bundle exec rake conformance:draft7
```

For a concise pass/fail summary:

```sh
bundle exec rake conformance:draft7:stats
```

Known failing tests can be tracked in `spec/conformance/draft7_pending.yml`; tests listed there are reported as `pending` instead of failing the build. To see a category breakdown of pending entries:

```sh
bundle exec rake conformance:draft7:gaps
```

### Current pass rate

<!-- conformance:start -->
**Draft-7**: 1457 / 1584 (92.0%) — 127 pending (25 intentionally ignored, 102 open), 0 failing.

Remote-ref resolution (`refRemote.json`) is intentionally not implemented and contributes to the failing count.
<!-- conformance:end -->

## Benchmarks

> **`fast_json-schema` is currently ~20x faster than `json_schemer`** on the
> bundled fixtures. See the numbers below for the exact breakdown.

`fast_json-schema` is a JSON Schema validator implemented as a Ruby C extension, and one of its core goals is to be meaningfully faster than the pure-Ruby alternatives for the Draft-7 keywords it supports.

The repository ships a benchmark script that compares against [`json_schemer`](https://github.com/davishmcclurg/json_schemer) on representative fixtures under [`data/`](data/) — a schema, a valid instance, and an instance with a single validation error.

To reproduce locally:

```sh
bin/benchmark
```

The script:

1. Boots both validators against the same compiled schema.
2. Confirms that both validators agree on the validity of each fixture before benchmarking (so we are comparing apples to apples).
3. Runs [`benchmark-ips`](https://github.com/evanphx/benchmark-ips) to measure sustained validation throughput for both the valid and invalid payloads.
4. Runs [`memory_profiler`](https://github.com/SamSaffron/memory_profiler) over **1_000** iterations of each scenario to capture allocation pressure.

Absolute throughput depends on the host's CPU, Ruby version, and JIT configuration. The ratio between the two validators is the meaningful figure; treat the absolute numbers as a snapshot, not a contract.

### Environment

- **Hardware**: Apple M4 Max
- **OS**: macOS Tahoe 26.3.1
- **Ruby**: 3.3.10 (arm64-darwin24)
- **`fast_json-schema`**: 0.1.0
- **`json_schemer`**: 2.4.0
- **Date**: 2026-05-19

### Validation throughput (`benchmark-ips`)

#### `valid?` on a valid payload

```
=== IPS: valid?(VALID) ===
ruby 3.3.10 (2025-10-23 revision 343ea05002) [arm64-darwin24]
Warming up --------------------------------------
    fast_json-schema     5.379k i/100ms
        json_schemer   258.000 i/100ms
Calculating -------------------------------------
    fast_json-schema     53.136k (± 2.1%) i/s   (18.82 μs/i) -    268.950k in   5.063959s
        json_schemer      2.555k (± 1.7%) i/s  (391.32 μs/i) -     12.900k in   5.049376s

Comparison:
    fast_json-schema:    53135.8 i/s
        json_schemer:     2555.5 i/s - 20.79x  slower
```

#### `valid?` on an invalid payload

```
=== IPS: valid?(INVALID) ===
ruby 3.3.10 (2025-10-23 revision 343ea05002) [arm64-darwin24]
Warming up --------------------------------------
    fast_json-schema    23.353k i/100ms
        json_schemer     8.887k i/100ms
Calculating -------------------------------------
    fast_json-schema    265.001k (±11.4%) i/s    (3.77 μs/i) -      1.308M in   5.008035s
        json_schemer     88.905k (± 1.5%) i/s   (11.25 μs/i) -    453.237k in   5.099150s

Comparison:
    fast_json-schema:   265001.2 i/s
        json_schemer:    88904.7 i/s - 2.98x  slower
```

### Memory (`memory_profiler`, 1 000 iterations of `valid?`)

#### Valid payload

```
=== Memory: valid?(VALID) x 1000 ===

--- fast_json-schema ---
Total allocated: 18.97 MB (58000 objects)
Total retained:  831.00 B (3 objects)

--- json_schemer ---
Total allocated: 138.62 MB (1535000 objects)
Total retained:  0 B (0 objects)
```

#### Invalid payload

```
=== Memory: valid?(INVALID) x 1000 ===

--- fast_json-schema ---
Total allocated: 9.10 MB (4000 objects)
Total retained:  0 B (0 objects)

--- json_schemer ---
Total allocated: 6.87 MB (72000 objects)
Total retained:  0 B (0 objects)
```

### Notes

- The benchmark script pins `json_schemer` via an inline `bundler/inline` gemfile, so reproductions don't perturb the project's primary `Gemfile.lock`.
- Both libraries are configured with the same schema and feed; the numbers reflect end-to-end `valid?` calls (no compilation in the hot loop — the schema is compiled once outside the measurement block).

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake` to compile the extension and run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/meinac/fast_json-schema. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/meinac/fast_json-schema/blob/master/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the FastJson::Schema project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/fast_json-schema/blob/master/CODE_OF_CONDUCT.md).
