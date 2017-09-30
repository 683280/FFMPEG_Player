package com.carljay.ffmpeg_project

import android.view.Surface

/**
 * Created by 13342 on 2017/9/29.
 */
class MediaPlayer{
    companion object {
        init {
            System.loadLibrary("ffmpeg_jni-lib")
        }
    }
    //保存jni层的映射对象
    var jni_object:Long = -1
    var _path:String? = null
    private external fun _start():Int
    private external fun _load():Int
    private external fun _pause():Int
    private external fun _stop():Int
    private external fun _setSurface(surface:Any,width:Int,height:Int):Int
    private external fun _setPath(path:String):Int
    private external fun _seek(path:Int):Int
    fun start():Int{
        _start()
        return 0
    }
    fun load():Int{
        Thread{
            _load()
        }.start()

        return 0
    }
    fun pause():Int{
        _pause()
        return 0
    }
    fun stop():Int{
        _stop()
        return 0
    }
    fun setSurface(surface: Surface,width:Int,height:Int){
        _setSurface(surface,width,height)
    }
    fun setPath(path:String){
        _path = path
        _setPath(_path!!)
    }
    fun seek(position:Int):Int{
        _seek(position)
        return 0
    }
}