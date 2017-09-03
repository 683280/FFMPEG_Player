package com.carljay.ffmpeg_project.ffmpeg

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

/**
 * Created by carljay on 17-9-3.
 */
class VideoPlayer (context: Context,attrs: AttributeSet?): SurfaceView(context),SurfaceHolder.Callback {

    override fun surfaceChanged(p0: SurfaceHolder?, p1: Int, p2: Int, p3: Int) {

    }

    override fun surfaceDestroyed(p0: SurfaceHolder?) {
    }

    override fun surfaceCreated(p0: SurfaceHolder?) {
        ffmpeg_object.setSurface(holder.surface)
    }

    var ffmpeg_object = FFmpegObject()
    init {
        holder.addCallback(this)
    }

    fun play() {
        ffmpeg_object.start()
    }
}

