package com.carljay.ffmpeg_project.ffmpeg

import android.media.MediaRecorder
import android.view.Surface

/**
 * Created by 13342 on 2017/9/20.
 */
class FFmpegRecord{
    external fun a() : Int
    var record : MediaRecorder? = null
    init {
        record = MediaRecorder()
    }
    fun setSurface(surface: Surface){
    }
}