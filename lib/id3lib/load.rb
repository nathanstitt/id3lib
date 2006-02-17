require 'id3lib'
require 'musicextras/song'
require 'musicextras/musicsites/_load_sites'


MusicExtras::MusicSite::activate_plugins

class ID3lib

class Load 



    def Load.from_file( path )

        @site = MusicExtras::LyricsTime.new

        puts  @site.lyrics( MusicExtras::Song.new('I Palindrome I','They Might Be Giants' ) )
    end



end # Load

end # ID3lib
