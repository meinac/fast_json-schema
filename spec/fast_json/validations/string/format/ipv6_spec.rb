# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format ipv6" do
    it_behaves_like "string format", "ipv6", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "2001:0db8:0000:0000:0000:0000:0000:0001" | true
        "2001:db8:0:0:0:0:0:1"                    | true
        "fe80:0:0:0:0:0:0:1"                      | true
        "2001:db8::1"                             | true
        "::1"                                     | true
        "::"                                      | true
        "1::"                                     | true
        "::ffff"                                  | true
        "2001:db8::"                              | true
        "fe80::1"                                 | true
        "::ffff:192.168.0.1"                      | true
        "2001:db8::192.168.0.1"                   | true
        "::192.168.0.1"                           | true
        "2001:DB8::1"                             | true
        "ABCD:ef01:2345:6789:abcd:ef01:2345:6789" | true
        ""                                        | false
        ":"                                       | false
        ":::"                                     | false
        "1::2::3"                                 | false
        "1:2:3:4:5:6:7:8:9"                       | false
        "1:2:3:4:5:6:7"                           | false
        "1:2:3:4:5:6:7:"                          | false
        ":1:2:3:4:5:6:7:8"                        | false
        "gggg::1"                                 | false
        "2001:db8::g"                             | false
        "12345::1"                                | false
        "::192.168.0.256"                         | false
        "::192.168.0"                             | false
        "1:2:3:4:5:6:192.168.0.1.5"               | false
        "192.168.0.1"                             | false
        "127.0.0.1"                               | false
      end
    }
  end
end
