package com.carljay.ffmpeg_project.ffmpeg

import android.util.Log
import android.view.Surface
/**
 * Created by carljay on 17-9-3.
 */
class FFmpegObject{
    var jni_object:Long = 0
    external fun _start(i:java.lang.Integer):java.lang.Integer
//    external fun _pause()
    fun setSurface(window : Surface){

    }
    companion object {
        init {
            System.loadLibrary("ffmpeg_jni-lib")
        }
    }

    fun start() {
        Log.e("TAG", "start = ${_start(Integer(0)) }")
    }
}