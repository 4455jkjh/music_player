package ffmpeg.audio;

import android.app.*;
import android.os.*;
import android.view.*;
import android.widget.*;

public class MainActivity extends Activity
{
	audio a;
	EditText et;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
	{
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
		et=(EditText) findViewById(R.id.path);
		//String path=getIntent().getStringExtra("path");
		a=new audio(null);
		
    }
	public void click(View v){
		a.release();
		a.setData(et.getText().toString());
		a.play();
	}
	@Override
	public void finish()
	{
		// TODO: Implement this method
		super.finish();
	}
	
}
