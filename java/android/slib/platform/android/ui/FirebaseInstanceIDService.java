package slib.platform.android.ui;

import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.FirebaseInstanceIdService;

public class FirebaseInstanceIDService extends FirebaseInstanceIdService{
    @Override
    public void onTokenRefresh() {
        String refreshedToken = FirebaseInstanceId.getInstance().getToken();
        nativeOnTokenRefresh(refreshedToken);
    }

    static public void getToken() {
        String token = FirebaseInstanceId.getInstance().getToken();
        if (token != null && token.length() > 0) {
            nativeOnTokenRefresh(token);
        }
    }

    private static native void nativeOnTokenRefresh(String token);
}