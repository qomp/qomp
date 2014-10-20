package net.sourceforge.qomp;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Binder;

import android.util.Log;

//for notification icon
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Notification;
import android.support.v4.app.NotificationCompat;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;

//for Toast
import android.widget.Toast;
import java.lang.Runnable;
import android.os.Handler;



public class QompService extends Service {

    private static final int NotifRef = 1;
    private final IBinder binder = new QompBinder();
    private final Handler handler = new Handler();

    @Override
    public void onCreate() {
        super.onCreate();
//        Log.i("QompService", "onCreated");
    }

    @Override
    public void onDestroy() {
//        Log.i("QompService", "onDestroy");
        stopForeground(true);
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
//        Log.i("QompService", "onBind");
        return binder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
//        Log.i("QompService", "onUnbind");
        return false;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
//        Log.i("QompService", "onStartCommand");
//        if(intent != null) {
//            showStatusIcon("");
//            return Service.START_STICKY;
//        }
        return Service.START_NOT_STICKY;
    }

    public void showStatusIcon(final String text) {
//        Log.i("QompService", "showStatusIcon  "+text);
        String app = getResources().getString(R.string.app_name) + " - " +
                     getResources().getString(R.string.icon_info);
        Bitmap icon = BitmapFactory.decodeResource(this.getResources(),
                                                   R.drawable.notification);
        NotificationCompat.Builder b = new NotificationCompat.Builder(this)
                        .setContentTitle(app)
                        .setContentText(text)
                        .setSmallIcon(R.drawable.notification);
                        //.setLargeIcon(icon);

        Intent notificationIntent = new Intent(this, Qomp.class);
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                                       notificationIntent,
                                       PendingIntent.FLAG_UPDATE_CURRENT |
                                       Intent.FLAG_ACTIVITY_NEW_TASK);

         b.setContentIntent(contentIntent);
         Notification not = b.build();
         not.flags = not.flags | Notification.FLAG_ONGOING_EVENT;
         startForeground(NotifRef, not);
    }

    private class ToastRunnable implements Runnable {
        String text_;

        public ToastRunnable(final String text) {
            text_ = text;
        }

        @Override
        public void run(){
             Toast.makeText(getApplicationContext(), text_, Toast.LENGTH_LONG).show();
        }
    }

    public void showToast(final String text) {
        if(text.length() == 0)
            return;

        handler.post(new ToastRunnable(text));
    }

    public class QompBinder extends Binder {
        QompService getService() {
            return QompService.this;
        }
    }
}
