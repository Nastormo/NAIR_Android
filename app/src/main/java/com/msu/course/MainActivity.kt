package com.msu.course

import android.content.pm.ActivityInfo
import android.content.res.AssetManager
import android.os.Bundle
import com.msu.course.databinding.ActivityMainBinding
import android.view.Surface
import android.view.SurfaceHolder
import android.view.View


class MainActivity : android.app.Activity(), SurfaceHolder.Callback2 {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        window.takeSurface ( this )

        requestedOrientation =  (ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        @Suppress ( "DEPRECATION" )
        window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_FULLSCREEN or
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY

        doCreate (assets)
    }

    /**
     * A native method that is implemented by the 'course' native library,
     * which is packaged with this application.
     */
    private external fun doCreate (assetManager : AssetManager)
    private external fun doSurfaceCreated ( surface : Surface )

    companion object {
        // Used to load the 'course' library on application startup.
        init {
            System.loadLibrary("course")
        }
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        doSurfaceCreated ( holder.surface )
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        // TODO("Not yet implemented")
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        // TODO("Not yet implemented")
    }

    override fun surfaceRedrawNeeded(holder: SurfaceHolder) {
        // TODO("Not yet implemented")
    }
}