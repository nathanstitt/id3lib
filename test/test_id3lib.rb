#!/usr/bin/ruby

DIR=File.dirname(__FILE__)

$:.unshift DIR+"/../ext/id3lib"
$:.unshift DIR+"/../lib"

require 'test/unit'
require 'id3lib'

class ID3LIB_TEST < Test::Unit::TestCase

    def read_id3
        ID3lib.new( DIR+'/test.mp3' )
    end

    def test_attrs
        id3=read_id3
        id3.each_possible_tag do | tag,desc |
            if id3.method( tag ).call.is_a?( Numeric )
                assert( id3.method( tag+'=' ).call( 42 ) )
            else
                assert( id3.method( tag+'=' ).call( tag.capitalize + " Test" ) )
            end
        end
        id3.save

        id3_2=read_id3
        id3_2.each_possible_tag do | tag,desc |
            if id3_2.method( tag ).call.is_a?( Numeric )
                assert_equal( 42, id3_2.method( tag ).call )
            else
                assert_equal( tag.capitalize + " Test" , id3_2.method( tag ).call )
            end
        end
     end

    def test_download
        tag=read_id3
        tag.each_picture do | pic | 
            pic.delete
        end
        tag.title='Holiday'
        tag.album='American Idiot'
        tag.artist='Green Day'
        tag.download_metadata
        assert( ! tag.pictures.empty? )
    end

    def test_pictures
        id3=read_id3
        id3.each_picture do | pic | 
            assert_kind_of( String, pic.description )
            pic.delete
        end
        id3.save

        id3=read_id3
        assert( id3.pictures.empty? )

        id3=read_id3
        data=''
        File.open( DIR+'/cover.jpg' ){ | f | data=f.read }
        pic=id3.add_picture( data, ID3lib::Picture::FRONT_COVER )
        pic.description='Test Picture'
        id3.save

        id3=read_id3
        assert_equal( 1, id3.pictures.size )
        assert_equal( 'Test Picture', id3.pictures.first.description )
        assert_equal( data, id3.pictures.first.data )
        assert_equal( "FRONT_COVER", id3.pictures.first.depicts )
    end
end

