
begin
    require 'musicextras/mconfig'
    $LOAD_PATH.unshift(MusicExtras::MConfig.instance['basedir'])
    require 'musicextras/musicsites/_load_sites'
    require 'musicextras/musicsite'
    require 'musicextras/album'
    require 'musicextras/artist'
    require 'musicextras/song'
    require 'musicextras/cache'
    require 'musicextras/guicontrol'
    require 'musicextras/debuggable'
    require 'base64'
    require 'ostruct'
    require 'fileutils'
    MusicExtras::MusicSite::activate_plugins( MusicExtras::MusicSite::plugins )    
    HAVE_MUSIC_EXTRAS=true
rescue LoadError
    HAVE_MUSIC_EXTRAS=false
end

class ID3lib



class Load 

    def Load.from_tag( tag )
        return tag unless HAVE_MUSIC_EXTRAS

        if tag.album && ! tag.album.empty?
            album=MusicExtras::Album.new( tag.album,  tag.artist  )
            cover=album.cover
            if cover
                tag.each_picture do | pic |
                    pic.delete if pic.depicts == ID3lib::Picture::FRONT_COVER
                end
                tag.add_picture( cover, ID3lib::Picture::FRONT_COVER )
                song=MusicExtras::Song.with_album( tag.title, album )
            else
                song=MusicExtras::Song.new( tag.title, tag.artist )
            end
        end
        song=MusicExtras::Song.new( tag.title, tag.artist )
        synced=song.synced_lyrics( song )
        tag.synced_lyrics=synced if synced
        tag.unsynced_lyrics=song.lyrics if song.lyrics
        return tag
    end

    def Load.from_file( path )
        id3=ID3lib.new( path )
        return Load.from_tag( id3 )
    end


end # Load

end # ID3lib
