
begin
    require 'musicextras/song'
    require 'musicextras/musicsites/_load_sites'
    MusicExtras::MusicSite::activate_plugins
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
        else
            song=MusicExtras::Song.new( tag.title, tag.artist )
        end
        lr=MusicExtras::Lrcdb.new
        synced=lr.synced_lyrics( song )
        if synced
            tag.synced_lyrics=synced
        elsif song.lyrics
            tag.unsynced_lyrics=song.lyrics
        end
        return tag
    end

    def Load.from_file( path )
        id3=ID3lib.new( path )
        return Load.from_tag( id3 )
    end


end # Load

end # ID3lib
