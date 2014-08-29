package com.googlecode.qomp;

import android.content.Context;
import android.content.Intent;
import android.app.Activity;
import java.lang.Exception;

import android.util.Log;

//for menu key handling
import android.view.KeyEvent;

//for Toast
//import android.widget.Toast;
//import java.lang.Thread;
//import java.lang.Runnable;

//import java.util.Locale;

//import android.content.res.Configuration;
import android.os.Bundle;

//for registering menu key
import android.view.ViewConfiguration;
import android.view.WindowManager;
import android.os.Build;

//for incomming calls receiving
import android.telephony.TelephonyManager;
import android.content.BroadcastReceiver;
import android.os.Bundle;
import android.content.IntentFilter;

//for notification icon
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Notification;
import android.support.v4.app.NotificationCompat;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;


public class Qomp extends org.qtproject.qt5.android.bindings.QtActivity {
    public static final String NOTIFY = "com.googlecode.qomp.NOTIFY";

    private static Qomp _instance;
    private BroadcastReceiver callReceiver_;
    private BroadcastReceiver notifyReceiver_;
    private static final int NotifRef = 1;
    private static NotificationManager manager_ = null;

    private static NotificationManager getManager() {
        if(manager_ == null) {
            manager_ = (NotificationManager)_instance.getSystemService(Context.NOTIFICATION_SERVICE);
        }
        return manager_;
    }

    @Override
    public void onCreate (Bundle savedInstanceState) {
        // Log.i("Qomp", "onCreated");
        super.onCreate(savedInstanceState);
        _instance = this;
        checkMenuKey();
        registerCallReceiver();
        registerNotyfyReceiver();
        showStatusIcon("");
    }

    @Override
    public void onDestroy() {
        //Log.i("Qomp", "onDestroy");
        getManager().cancel(NotifRef);
        unregisterReceiver(callReceiver_);
        unregisterReceiver(notifyReceiver_);
        super.onDestroy();
    }

    //http://stackoverflow.com/questions/17667245/how-to-show-option-menu-in-android-4-2
    //http://stackoverflow.com/questions/14068138/alternate-to-haspermanentmenukey-for-android-2-3-3
    private void checkMenuKey() {
        // set option menu if has no hardware menu key
        int lvl = Integer.valueOf(android.os.Build.VERSION.SDK);
        boolean hasMenu;
        if(lvl > 13) {
            hasMenu = ViewConfiguration.get(this).hasPermanentMenuKey();
        }
        else if(lvl < 11) {
            hasMenu = true;
        }
        else {
            hasMenu = false;
        }

        if(!hasMenu){
                //getWindow().setFlags(0x08000000, 0x08000000);
            try {
                getWindow().addFlags(WindowManager.LayoutParams.class.getField("FLAG_NEEDS_MENU_KEY").getInt(null));
            }
            catch (NoSuchFieldException e) {
                // Ignore since this field won't exist in most versions of Android
            }
            catch (IllegalAccessException e) {
                Log.w("Qomp", "Could not access FLAG_NEEDS_MENU_KEY in addLegacyOverflowButton()", e);
            }
        }
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
       // Log.i("Qomp", "onKeyDown");
        if (keyCode == KeyEvent.KEYCODE_MENU) {
            menuKeyDown();
        }
        return super.onKeyDown(keyCode, event);
    }

    private void registerCallReceiver() {
        callReceiver_ = new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent) {
                    Bundle bundle = intent.getExtras();

                    if(null == bundle)
                            return;

                    String state = bundle.getString(TelephonyManager.EXTRA_STATE);
                    processIncomingCall(state);
            }
        };
        IntentFilter f = new IntentFilter("android.intent.action.PHONE_STATE");
        registerReceiver(callReceiver_, f);
    }

    private void registerNotyfyReceiver() {
        notifyReceiver_ = new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent) {
                //Log.i("Qomp", "Broadcast");
                Intent i = new Intent(getBaseContext(), Qomp.class);
                i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                getBaseContext().startActivity(i);
            }
        };
        IntentFilter f = new IntentFilter(Qomp.NOTIFY);
        registerReceiver(notifyReceiver_, f);
    }

//    private static void showToast(int resourceId)
//    {
//        final String message = _instance.getResources().getString(resourceId);

//        new Thread()
//        {
//            public void run()
//            {
//                _instance.runOnUiThread(new Runnable() {

//                    public void run() {
//                        Toast.makeText(_instance, message, Toast.LENGTH_LONG).show();
//                    }
//                });
//            }
//        }.start();
//    }

    public static void processIncomingCall(final String state) {
       // Log.i("Qomp","State: "+ state);

        if(state.equalsIgnoreCase(TelephonyManager.EXTRA_STATE_RINGING))
        {
                //Log.i("Qomp","Incomng Call");
                incomingCallStart();
        }

        if(state.equalsIgnoreCase(TelephonyManager.EXTRA_STATE_IDLE))
        {
               // Log.i("Qomp","Incomng Call Finished");
                incomingCallFinish();
        }
    }

    public static void showStatusIcon(final String text) {
        String app = _instance.getResources().getString(R.string.app_name) + " - " +
                     _instance.getResources().getString(R.string.icon_info);;
        Bitmap icon = BitmapFactory.decodeResource(_instance.getResources(),
                                                   R.drawable.notification);
        NotificationCompat.Builder b = new NotificationCompat.Builder(_instance)
                        .setContentTitle(app)
                        .setContentText(text)
                        .setSmallIcon(R.drawable.notification);
                        //.setLargeIcon(icon);


        // This is what you are going to set a pending intent which will start once
        // notification is pressed. Hopes you know how to add notification bar.
        Intent notificationIntent = new Intent(_instance, CallbackActivity.class);
        PendingIntent contentIntent = PendingIntent.getActivity(_instance, 0,
                                       notificationIntent,
                                       PendingIntent.FLAG_UPDATE_CURRENT |
                                       Notification.FLAG_AUTO_CANCEL);

         b.setContentIntent(contentIntent);
         Notification not = b.build();
         not.flags = not.flags | Notification.FLAG_ONGOING_EVENT;
         getManager().notify(NotifRef, not);
    }

    private static native void menuKeyDown();
    private static native void incomingCallStart();
    private static native void incomingCallFinish();
}
