package net.sourceforge.qomp;

import android.content.Context;
import android.content.Intent;
import android.app.Activity;
import java.lang.Exception;

//for checking intent
import android.os.Handler;

import android.util.Log;

//for menu key handling
import android.view.KeyEvent;

//import java.util.Locale;

//import android.content.res.Configuration;
import android.os.Bundle;

//for incomming calls receiving
import android.telephony.TelephonyManager;
import android.content.BroadcastReceiver;
import android.os.Bundle;
import android.content.IntentFilter;

//for service
import android.content.ServiceConnection;
import android.os.IBinder;
import android.content.ComponentName;
import android.app.Service;

//For PowerManager
//import android.os.PowerManager;
//import android.os.PowerManager.WakeLock;


public class Qomp extends org.qtproject.qt5.android.bindings.QtActivity {
    public static final String NOTIFY = "net.sourceforge.qomp.NOTIFY";

//    private PowerManager.WakeLock wl;
    private BroadcastReceiver callReceiver_;
    private QompService service_;

    private ServiceConnection sConn_ = new ServiceConnection() {
            public void onServiceConnected(ComponentName name, IBinder binder) {
                service_ = ((QompService.QompBinder)binder).getService();
                showStatusIcon("");
            }

            public void onServiceDisconnected(ComponentName name) {
                service_ = null;
            }
    };

    @Override
    public void onCreate (Bundle savedInstanceState) {
        // Log.i("Qomp", "onCreated");
        super.onCreate(savedInstanceState);
        registerCallReceiver();
        bindToService();

//        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
//        PowerManager.WakeLock wl = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "Tag");
//        wl.acquire();

    }

    public String checkIntent() {
        Intent intent = getIntent();
        if(intent != null) {
            String action = intent.getAction();
            if (action != null && action.compareTo(Intent.ACTION_VIEW) == 0) {
                String uri = intent.getDataString();
                return uri;
            }
        }
        return null;
    }

    @Override
    public void onDestroy() {
        //Log.i("Qomp", "onDestroy");
        super.onDestroy();
    }

    public void deInit() {
        unbindService(sConn_);
        unregisterReceiver(callReceiver_);
//        wl.release();
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

    private void bindToService() {
        Intent i = new Intent(this, QompService.class);
        bindService(i, sConn_, Context.BIND_AUTO_CREATE);
    }


    public void showStatusIcon(final String text) {
        if(service_ != null)
            service_.showStatusIcon(text);
    }

    public void showNotification(final String text) {
        if(service_ != null)
            service_.showToast(text);
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        String uri = intent.getDataString();
//        Log.i("Qomp","onNewInten " + uri);
        if (uri != null){
            setUrl(uri);
        }
    }

    private static native void menuKeyDown();
    private static native void incomingCallStart();
    private static native void incomingCallFinish();
    private static native void setUrl(final String url);
}
