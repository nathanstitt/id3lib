#!/usr/bin/ruby

DIR=File.dirname(__FILE__)

$:.unshift DIR+"/../ext/id3lib"
$:.unshift DIR+"/../lib"

require 'id3lib'

tag=ID3lib.new( DIR+'/test.mp3' )
tag.artist='They Might Be Giants'
tag.title='I Palindrome I'
tag.save

#ID3lib::Load.from_file( DIR+'/test.mp3' )

tag = ID3lib::Load.from_file( "/mnt/snoopy/music/Tom/Pink Floyd - Echoes Disk 1/05 - Track 5 - .mp3" )

puts tag.pictures.size

__END__

class ID3LIB_TEST < Test::Unit::TestCase


end
