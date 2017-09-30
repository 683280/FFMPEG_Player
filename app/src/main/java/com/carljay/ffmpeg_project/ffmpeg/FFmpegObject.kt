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
    external fun _load():Int
    external fun _setSurface(surface:Any,width:Int,height :Int):Int
//    external fun _pause()
    fun setSurface(window : Surface,width:Int,height:Int){
        _setSurface(window,width,height)
    }
    fun setPath(){

    }
    fun start(i :Int) {
        _setPath("http://192.168.100.104/1.flv")
        Thread{
            _load()
        }.start()
        Thread{
            Log.e("TAG", "jni_object = $jni_object")
            Log.e("TAG", "start = ${_start() }")
        }.start()
    }
}