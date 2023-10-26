# FastJson::Schema

Validates JSON instances against given JSON schema.

## Installation

Install the gem and add to the application's Gemfile by executing:

    $ bundle add fast_json-schema

If bundler is not being used to manage dependencies, install the gem by executing:

    $ gem install fast_json-schema

## Usage

TODO: Write usage instructions here

## Roadmap

This gem is under development at the moment and I am planning to support only the Draft 7 in the first version. That being said, some of the validation keywords of Draft 8 are implemented. Here is the list of keywords/annotations and the progress;

- **Annotations**
    - `$id` &check;
    - `$ref` &cross;
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
    - `dependencies` &cross;
- **Formats**
    - Enabling/disabling the `format` as assertion &cross;
    - `Custom format attributes` &cross;
    - Dates, Times, and Duration
        - `date-time` &cross;
        - `date` &cross;
        - `time` &cross;
        - `duration` &cross; (Draft 8)
    - Email Addresses
        - `email` &cross;
        - `idn-email` &cross;
    - Hostnames
        - `hostname` &cross;
        - `idn-hostname` &cross;
    - IP Addresses
        - `IPv4` &cross;
        - `IPv6` &cross;
    - Resource Identifiers
        - `uri` &cross;
        - `uri-reference` &cross;
        - `iri` &cross;
        - `iri-reference` &cross;
        - `uuid` &cross; (Draft 8)
    - `uri-template` &cross;
    - JSON Pointers
        - `json-pointer` &cross;
        - `relative-json-pointer` &cross;
    - `regex` &cross;
- **Vocabulary for the Contents of String-Encoded Data**
    - I'm not planning to support validating string instances with these annotations in the near future.

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake` to compile the extension and run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/meinac/fast_json-schema. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/meinac/fast_json-schema/blob/master/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the FastJson::Schema project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/fast_json-schema/blob/master/CODE_OF_CONDUCT.md).
