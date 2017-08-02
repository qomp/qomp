package net.sourceforge.qomp;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.util.Log;
import android.view.KeyEvent;

public class MediaButtonReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        String intentAction = intent.getAction();
       // Log.i ("QompBroadcastReceiver", intentAction.toString() + " happended");

        if (Intent.ACTION_MEDIA_BUTTON.equals(intentAction)) {
            KeyEvent event = (KeyEvent)intent.getParcelableExtra(Intent.EXTRA_KEY_EVENT);
            if (event != null) {
                if (event.getAction() == KeyEvent.ACTION_UP) {
                    Qomp.mediaButtonClicked();
                }
                abortBroadcast();
            }
        }
    }

}
