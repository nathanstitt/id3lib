
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

        unless tag.album.empty? || tag.has_picture?( ID3lib::Picture::FRONT_COVER )
            album=MusicExtras::Album.new( tag.album,  tag.artist  )
            cover=album.cover
            if cover
                tag.add_picture( cover, ID3lib::Picture::FRONT_COVER )
            end
        end
        unless tag.artist.empty?
            unless tag.title.empty?
                song=MusicExtras::Song.new( tag.title, tag.artist )
                if tag.synced_lyrics.empty?
                    lyr=song.synced_lyrics
                    tag.synced_lyrics=lyr if lyr
                end
                if tag.lyrics.empty?
                    lyr=song.lyrics
                    tag.lyrics=lyr if lyr
                end
            end
            artist=MusicExtras::Artist.new( tag.artist )
            unless tag.has_picture? ID3lib::Picture::ARTIST
                image=artist.image  
                if image
                    tag.add_picture( image, ID3lib::Picture::FRONT_COVER )                    
                end
            end
        end
        return tag
    end

    def Load.from_file( path )
        id3=::ID3lib.new( path )
        return Load.from_tag( id3 )
    end


end # Load

end # ID3lib
