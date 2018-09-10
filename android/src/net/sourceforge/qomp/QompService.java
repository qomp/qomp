package net.sourceforge.qomp;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.widget.Toast;

public class QompService extends Service {

    private static final int NotifRef = 1;
    private static final String ChannelId = "QompServiceChannel";
    private final IBinder binder = new QompBinder();
    private final Handler handler = new Handler();

    @Override
    public void onCreate() {
        super.onCreate();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(ChannelId,
                    getResources().getString(R.string.channel_name),
                    NotificationManager.IMPORTANCE_DEFAULT);
            channel.setSound(null, null);
            NotificationManager m = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            if(m != null)
                m.createNotificationChannel(channel);
        }
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
//        Bitmap icon = BitmapFactory.decodeResource(this.getResources(),
//                                                   R.drawable.notification);
        NotificationCompat.Builder b = new NotificationCompat.Builder(this, ChannelId)
                        .setContentTitle(app)
                        .setContentText(text)
                        .setSmallIcon(R.drawable.notification);
                        //.setLargeIcon(icon);

        Intent notificationIntent = new Intent(this, Qomp.class);
        notificationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                                       notificationIntent,
                                       PendingIntent.FLAG_UPDATE_CURRENT);

         b.setContentIntent(contentIntent);
         Notification not = b.build();
         not.flags = not.flags | Notification.FLAG_ONGOING_EVENT;
         startForeground(NotifRef, not);
    }

    private class ToastRunnable implements Runnable {
        private String text_;

        ToastRunnable(final String text) {
            text_ = text;
        }

        @Override
        public void run() {
             Toast.makeText(getApplicationContext(), text_, Toast.LENGTH_LONG).show();
        }
    }

    public void showToast(final String text) {
        if(text.length() == 0)
            return;

        handler.post(new ToastRunnable(text));
    }

    class QompBinder extends Binder {
        QompService getService() {
            return QompService.this;
        }
    }
}
