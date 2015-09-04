package ffmpeg.audio;
import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import java.io.File;
import android.view.KeyEvent;

public class playdialog extends Activity
{
	SeekBar seek;
	audio a;
	TextView curtime,totaltime;
	Handler h=new Handler(){
		@Override
		public void handleMessage(Message msg)
		{
			// TODO: Implement this method
			super.handleMessage(msg);
			if(msg.what==2){
				curtime.setText(list.gettime(a.getcur()));
				seek.setProgress((int)a.getcur());
			}
		}
	};
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		// TODO: Implement this method
		super.onCreate(savedInstanceState);
		setContentView(R.layout.control);
		seek=(SeekBar) findViewById(R.id.seek);
		curtime=(TextView)findViewById(R.id.curtime);
		totaltime=(TextView)findViewById(R.id.totaltime);
		Intent i=getIntent();
		if(i.getAction().equals(Intent.ACTION_VIEW)){
			Uri path=i.getData();
			a=new audio(h);
			a.setData(path.getPath());
			totaltime.setText(list.gettime(a.gettotal()));
			seek.setMax((int)a.gettotal());
			a.play();
		}
	}
	public void click(View v){
		
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		// TODO: Implement this method
		if(keyCode==4){
			a.release();
		}
		return super.onKeyDown(keyCode, event);
	}
	
}
