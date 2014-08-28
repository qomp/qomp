package com.googlecode.qomp;;

import android.content.Context;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.KeyEvent;
import android.provider.MediaStore;
import android.content.Intent;
import android.app.Activity;
import java.lang.Exception;
import android.net.Uri;
import android.database.Cursor;
//import android.view.Gravity;
import android.widget.Toast;
import java.lang.Thread;
import java.lang.Runnable;
import java.util.Locale;
import android.content.res.Configuration;
import java.io.File;
import android.os.Bundle;
import android.view.ViewConfiguration;
import android.view.WindowManager;

public class Qomp extends org.qtproject.qt5.android.bindings.QtActivity {

    private static Qomp _instance;

    @Override
    public void onCreate (Bundle savedInstanceState) {
        // Log.i("NpMobile", "onCreated");
        super.onCreate(savedInstanceState);
        _instance = this;
        checkMenuKey();
    }

    //http://stackoverflow.com/questions/17667245/how-to-show-option-menu-in-android-4-2
    private void checkMenuKey() {
        // set option menu if has no hardware menu key
        boolean hasMenu = ViewConfiguration.get(this).hasPermanentMenuKey();
        if(!hasMenu){
                //getWindow().setFlags(0x08000000, 0x08000000);
            try {
                getWindow().addFlags(WindowManager.LayoutParams.class.getField("FLAG_NEEDS_MENU_KEY").getInt(null));
            }
            catch (NoSuchFieldException e) {
                // Ignore since this field won't exist in most versions of Android
            }
            catch (IllegalAccessException e) {
                Log.w("NpMobile", "Could not access FLAG_NEEDS_MENU_KEY in addLegacyOverflowButton()", e);
            }
        }
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
       // Log.i("NpMobile", "onKeyDown");
        if (keyCode == KeyEvent.KEYCODE_MENU) {
            menuKeyDown();
        }
        return super.onKeyDown(keyCode, event);
    }

    private static void showToast(int resourceId)
    {
        final String message = _instance.getResources().getString(resourceId);

        new Thread()
        {
            public void run()
            {
                _instance.runOnUiThread(new Runnable() {

                    public void run() {
                        Toast.makeText(_instance, message, Toast.LENGTH_LONG).show();
                    }
                });
            }
        }.start();
    }


    private static native void menuKeyDown();
}
