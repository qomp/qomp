package net.sourceforge.qomp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.PowerManager;
import android.support.v4.content.ContextCompat;
import android.view.KeyEvent;

import com.google.firebase.analytics.FirebaseAnalytics;

import java.util.ArrayList;

public class Qomp extends org.qtproject.qt5.android.bindings.QtActivity
                  implements AudioManager.OnAudioFocusChangeListener
{
    private static final int PermissionsRequest = 1;
    private PowerManager.WakeLock wl_ = null;
//    private BroadcastReceiver callReceiver_;
    private QompService service_;
    private ComponentName mediaComponent_;
    private boolean mediaRegistered_ = false;

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
    public void onCreate(Bundle savedInstanceState) {
        // Log.i("Qomp", "onCreated");
        super.onCreate(savedInstanceState);

        checkRights();

        mediaComponent_ = new ComponentName(getPackageName(), MediaButtonReceiver.class.getName());

//        registerCallReceiver();
        bindToService();

        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        if(pm != null)
            wl_ = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "Tag");
    }

    @Override
    public void onResume() {
        super.onResume();
       // Log.e("Qomp", "onResume");

        unregisterMediaReceiver();
    }

    @Override
    public void onPause() {
        super.onPause();
       // Log.e("Qomp", "onPause");

        registerMediaReceiver();
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

    public String checkIntent() {
        Intent intent = getIntent();
        if(intent != null) {
            String action = intent.getAction();
            if (action != null && action.compareTo(Intent.ACTION_VIEW) == 0) {
                return intent.getDataString();
            }
        }
        return null;
    }

    public void deInit() {
        unbindService(sConn_);
//        unregisterReceiver(callReceiver_);
        unregisterMediaReceiver();
        if(wl_ != null && wl_.isHeld())
            wl_.release();
    }

    public void makeWakeLock(int timeout) {
//        Log.i("Qomp", "makeWakeLock " + Integer.toString(timeout));
        if(wl_ != null) {
            if (wl_.isHeld())
                wl_.release();

            wl_.acquire(timeout);
        }
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
       // Log.i("Qomp", "onKeyDown: " + String.valueOf(keyCode));
        if (keyCode == KeyEvent.KEYCODE_MENU) {
            menuKeyDown();
        }
        return super.onKeyDown(keyCode, event);
    }

    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // Log.i("Qomp", "onUpDown: " + String.valueOf(keyCode));
        if (keyCode == KeyEvent.KEYCODE_HEADSETHOOK) {
            mediaButtonClicked();
            return true;
        }
        return super.onKeyUp(keyCode, event);
    }

    /*private void registerCallReceiver() {
        callReceiver_ = new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent) {
                    Bundle bundle = intent.getExtras();

                    if(null == bundle)
                            return;

                    String state = bundle.getString(TelephonyManager.EXTRA_STATE);
                    if(state != null)
                        processIncomingCall(state);
            }
        };
        IntentFilter f = new IntentFilter("android.intent.action.PHONE_STATE");
        registerReceiver(callReceiver_, f);
    }*/

    private void registerMediaReceiver() {
        if(!mediaRegistered_) {
            AudioManager manager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
            if(manager != null) {
                int result = manager.requestAudioFocus(this, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);
                //if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
                manager.registerMediaButtonEventReceiver(mediaComponent_);
                mediaRegistered_ = true;
            }
            //}
        }
    }

    private void unregisterMediaReceiver() {
        if(mediaRegistered_) {
            AudioManager manager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
            if(manager != null) {
                manager.unregisterMediaButtonEventReceiver(mediaComponent_);
                mediaRegistered_ = false;
            }
        }
    }

    @Override
    public void onAudioFocusChange(int focusChange) {
        //Log.i("Qomp", "onAudioFocusChange " + String.valueOf(focusChange));
        if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT) {
            audioFocusLoss(true, false);
        } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK) {
            audioFocusLoss(true, true);
        } else if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
            audioFocusGain();
        } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS) {
            audioFocusLoss(false, false);
        }
    }

    /*public static void processIncomingCall(final String state) {
       // Log.i("Qomp","State: "+ state);

        if(state.equalsIgnoreCase(TelephonyManager.EXTRA_STATE_RINGING)) {
                //Log.i("Qomp","Incomng Call");
                incomingCallStart();
        }
        else if(state.equalsIgnoreCase(TelephonyManager.EXTRA_STATE_IDLE)) {
               // Log.i("Qomp","Incomng Call Finished");
                incomingCallFinish();
        }
    }*/

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

    public void logEvent(final String eventName, ArrayList<String> keys, ArrayList<String> values) {
        Bundle bundle = new Bundle();
        for(int i = 0; i < keys.size(); i++) {
            bundle.putString(keys.get(i), values.get(i));
        }
        FirebaseAnalytics.getInstance(this).logEvent(eventName, bundle);
    }

    @SuppressLint("NewApi")
    private void checkRights() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && (
                ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                /*|| ContextCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED*/) )
        {
                String[] perms = {Manifest.permission.WRITE_EXTERNAL_STORAGE/*, Manifest.permission.READ_PHONE_STATE*/};
                requestPermissions(perms, PermissionsRequest);
        }
    }

    private static native void menuKeyDown();
//    private static native void incomingCallStart();
//    private static native void incomingCallFinish();
    private static native void audioFocusLoss(boolean isTransient, boolean canDuck);
    private static native void audioFocusGain();
    private static native void setUrl(final String url);
    public  static native void mediaButtonClicked();
}
