package com.carljay.ffmpeg_project.ffmpeg

import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder

/**
 * Created by carljay on 17-9-3.
 */
class FFmpegObject{
    var jni_object:Long = 0
    external fun _start():Int
    external fun _setPath(path:String):Int
    external fun _setSurface(surface:Any,width:Int,height :Int):Int
//    external fun _pause()
    fun setSurface(window : Surface,width:Int,height:Int){
        _setSurface(window,width,height)
    }
    companion object {
        init {
            System.loadLibrary("ffmpeg_jni-lib")
        }
    }
    fun setPath(){

    }
    fun start(i :Int) {
        Thread{
            _setPath("http://baobab.wdjcdn.com/14564977406580.mp4")
            Log.e("TAG", "jni_object = $jni_object")
            Log.e("TAG", "start = ${_start() }")
            Log.e("TAG", "jni_object = $jni_object")

        }.start()

    }
}