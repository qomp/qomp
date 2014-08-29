package com.googlecode.qomp;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class CallbackActivity  extends Activity {

    @Override
       public void onCreate(Bundle savedInstanceState) {
           super.onCreate(savedInstanceState);
           Log.i("CallbackActivity", "oncreate");

           Intent i = new Intent();
           i.setAction(Qomp.NOTIFY);
           sendBroadcast(i);
           finish();
       }
}

