package net.sourceforge.qomp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.view.KeyEvent;

public class MediaButtonReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        String intentAction = intent.getAction();
        //Log.i ("QompBroadcastReceiver", intentAction.toString() + " happened");

        if (Intent.ACTION_MEDIA_BUTTON.equals(intentAction)) {
            KeyEvent event = intent.getParcelableExtra(Intent.EXTRA_KEY_EVENT);
            if (event != null) {
                if (event.getAction() == KeyEvent.ACTION_UP) {
                    Qomp.mediaButtonClicked();
                }
                abortBroadcast();
            }
        }
    }
}
