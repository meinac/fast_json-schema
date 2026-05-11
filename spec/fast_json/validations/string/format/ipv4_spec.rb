# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format ipv4" do
    it_behaves_like "string format", "ipv4", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "0.0.0.0"         | true
        "127.0.0.1"       | true
        "192.168.0.1"     | true
        "255.255.255.255" | true
        "1.2.3.4"         | true
        "10.0.0.0"        | true
        "256.0.0.1"       | false
        "1.2.3.256"       | false
        "999.999.999.999" | false
        "01.2.3.4"        | false
        "1.02.3.4"        | false
        "1.2.03.4"        | false
        "1.2.3.04"        | false
        "001.2.3.4"       | false
        "1.2.3"           | false
        "1.2.3.4.5"       | false
        ""                | false
        "1.2.3.a"         | false
        "1.2.3.-1"        | false
        "1.2.3.+4"        | false
        " 1.2.3.4"        | false
        "1.2.3.4 "        | false
        ".1.2.3"          | false
        "1..2.3"          | false
        "1.2.3."          | false
        "0xff.0.0.0"      | false
        "127.1"           | false
        "::1"             | false
      end
    }
  end
end
