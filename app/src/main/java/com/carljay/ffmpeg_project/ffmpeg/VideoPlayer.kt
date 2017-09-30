package com.carljay.ffmpeg_project.ffmpeg

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.carljay.ffmpeg_project.MediaPlayer

/**
 * Created by carljay on 17-9-3.
 */
class VideoPlayer (context: Context,attrs: AttributeSet?): SurfaceView(context,attrs,0),SurfaceHolder.Callback {

    override fun surfaceChanged(p0: SurfaceHolder?, p1: Int, p2: Int, p3: Int) {

    }

    override fun surfaceDestroyed(p0: SurfaceHolder?) {
    }

    override fun surfaceCreated(p0: SurfaceHolder?) {
        ffmpeg_object.setSurface(holder.surface,width,height)
    }

    var ffmpeg_object = MediaPlayer()
    init {
        holder.addCallback(this)
    }

    fun play() {
        ffmpeg_object.setPath( "http://192.168.100.104/1.flv")
        ffmpeg_object.load()
        ffmpeg_object.start()
    }
}

