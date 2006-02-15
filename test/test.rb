#!/usr/bin/ruby

$:.unshift File.dirname(__FILE__)+"/../"

require 'id3lib'
require 'test/unit'
tag=ID3lib.new( './test.mp3' )


class ID3LIB_TEST < Test::Unit::TestCase


    def set_tag_obj
        @tag=ID3lib.new( './test.mp3' )
    end

    def test_attrs
        set_tag_obj
        @tag.each_possible_tag do | tag |
puts tag
            if @tag.method( tag ).call.is_a?( Numeric )
                assert( @tag.method( tag+'=' ).call( 42 ) )
            else
                assert( @tag.method( tag+'=' ).call( tag.capitalize + " Test" ) )
            end
        end
        @tag.save
        set_tag_obj        
        @tag.each_possible_tag do | tag |
            if @tag.method( tag ).call.is_a?( Numeric )
                assert_equal( 42, @tag.method( tag ).call )
            else
                assert_equal( tag.capitalize + " Test" , @tag.method( tag ).call )
            end
        end
     end

end

