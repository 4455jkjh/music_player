package ffmpeg.audio;
import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Comparator;
import java.text.Collator;
import java.util.Locale;
import android.app.AlertDialog;
import android.widget.Button;

public class list extends Activity implements Runnable,AdapterView.OnItemClickListener
{
	List<File> lf;
	Compare compare;
	Adapter adapter;
	Intent intent;
	File cur,i;
	handler h;
	ListView listview;
	TextView curtime,totaltime;
	SeekBar seek;
	audio a;
	int index=0;
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		if(Build.VERSION.SDK_INT>=21){
			setTitleColor(0xffffffff);
		}
		// TODO: Implement this method
		super.onCreate(savedInstanceState);
		setContentView(R.layout.list);
		Window window = getWindow();
		if (Build.VERSION.SDK_INT == 19)
		{
			window.addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
			Class clazz = window.getClass();
			try
			{
				int tranceFlag = 0;
				int darkModeFlag = 0;
				Class layoutParams = Class.forName("android.view.MiuiWindowManager$LayoutParams");

				Field field = layoutParams.getField("EXTRA_FLAG_STATUS_BAR_TRANSPARENT");
				tranceFlag = field.getInt(layoutParams);

				field = layoutParams.getField("EXTRA_FLAG_STATUS_BAR_DARK_MODE");
				darkModeFlag = field.getInt(layoutParams);

				Method extraFlagField = clazz.getMethod("setExtraFlags", int.class, int.class); //只需要状态栏透明 extraFlagField.invoke(window, tranceFlag, tranceFlag); 或
				//状态栏透明且黑色字体
				extraFlagField.invoke(window,  darkModeFlag, tranceFlag | darkModeFlag);
				//清除黑色字体
				//extraFlagField.invoke(window, 0, darkModeFlag);
			}
			catch (NoSuchMethodException e)
			{
				e.printStackTrace();
			}
			catch (ClassNotFoundException e)
			{
				e.printStackTrace();
			}
			catch (NoSuchFieldException e)
			{
				e.printStackTrace();
			}
			catch (IllegalAccessException e)
			{
				e.printStackTrace();
			}
			catch (IllegalArgumentException e)
			{
				e.printStackTrace();
			}
			catch (InvocationTargetException e)
			{
				e.printStackTrace();
			}
		}
		listview = (ListView) findViewById(R.id.list);
		curtime = (TextView)findViewById(R.id.curtime);
		totaltime = (TextView)findViewById(R.id.totaltime);
		seek = (SeekBar) findViewById(R.id.seek);
		listview.setOnItemClickListener(this);
		lf = new ArrayList<File>();
		compare = new Compare();
		adapter = new Adapter();
		listview.setAdapter(adapter);
		i = new File("/sdcard/kgmusic");
		intent = new Intent(this, MainActivity.class);
		h = new handler();
		a = new audio(h);
		new Thread(this).start();
	}
	public void run()
	{
		update(i);
		h.sendEmptyMessage(0);
	}
	public void click(View v)
	{
		switch (v.getId())
		{
			case R.id.play:
				if (!a.isPlaying())
				{
					play();
					break;
				}
				a.play();
				break;
			case R.id.pause:
				a.pause();
				break;
			case R.id.next:
				index++;
				if (index == lf.size())
					index = 0;
				play();
				break;
			case R.id.previous:
				index--;
				if (index == -1)
					index = lf.size() - 1;
				play();
		}
	}
	private void update(File ff)
	{
		if (!ff.exists())return;
		for (File f:ff.listFiles())
		{
			if (f.isDirectory())update(f);
			for (String sub:subname)
			{
				if (f.getName().endsWith(sub))
				{
					lf.add(f);
				}
			}
		}
		Collections.sort(lf, compare);
		/*Collator cmp=Collator.getInstance(Locale.CHINESE);
		 Collections.sort(lf,cmp);*/
	} 
	public static String subname[]={
		".ape",
		".flac",
		".wav",
		".mp3",
		".mp2",
		".aac",
		".wav",
		".wma",
		".asf",
	};

	@Override
	public void onItemClick(AdapterView<?> p1, View p2, int p3, long p4)
	{
		// TODO: Implement this method
		// TODO: Implement this method
		index = p3;
		play();
	}
	private void play(File f){
		a.release();
		a.setData(f.getAbsolutePath());
		totaltime.setText(gettime(a.gettotal()));
		seek.setMax((int)a.gettotal());
		a.play();
		setTitle(f.getName());
	}
	private void play()
	{
		File f=lf.get(index);
		a.release();
		a.setData(f.getAbsolutePath());
		totaltime.setText(gettime(a.gettotal()));
		seek.setMax((int)a.gettotal());
		a.play();
		setTitle(f.getName());
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		// TODO: Implement this method
		switch (keyCode)
		{
			case KeyEvent.KEYCODE_MEDIA_NEXT:
				index++;
				if (index == lf.size())
					index = 0;
				play();
				return true;
			case KeyEvent.KEYCODE_MEDIA_PREVIOUS:
				index--;
				if (index == -1)
					index = lf.size() - 1;
				play();
				return true;
			case KeyEvent.KEYCODE_HEADSETHOOK:
				if (!a.isPlaying())a.pause();
				else a.play();
				return true;
			default:
				return super.onKeyDown(keyCode, event);
		}
	}

	private class Adapter extends BaseAdapter 
	{
		TextView size,time;
		@Override
		public int getCount()
		{
			// TODO: Implement this method
			return lf.size();
		}

		@Override
		public Object getItem(int p1)
		{
			// TODO: Implement this method
			return lf.get(p1);
		}
		@Override
		public long getItemId(int p1)
		{
			// TODO: Implement this method
			return p1;
		}
		@Override
		public View getView(int p1, View p2, ViewGroup p3)
		{
			// TODO: Implement this method
			File f=lf.get(p1);
			click c=new click(f);
			p2 = getLayoutInflater().inflate(R.layout.adapter, p3, false);
			TextView tv=(TextView)p2.findViewById(R.id.name);
			tv.setText(f.getName());
			tv.setOnClickListener(c);
			((Button)p2.findViewById(R.id.info)).setOnClickListener(c);
			/*size = (TextView)p2.findViewById(R.id.size);
			size.setText(size(f.length(), 0));*/
			return p2;
		}
	}
	class click implements View.OnClickListener
	{
		File s;
		public click(File s){
			this.s=s;
		}
		@Override
		public void onClick(View p1)
		{
			// TODO: Implement this method
			if(p1 instanceof Button){
				showinfo(s);
			}else if(p1 instanceof TextView){
				play(s);
			}
		}
	}
	String dw[]={
		"B",
		"KB",
		"MB",
		"GB",
		"TB",
	};
	public void showinfo(File s){
		AlertDialog.Builder ab=new AlertDialog.Builder(this);
		ab.setTitle(s.getName())
		.setMessage(s.getAbsolutePath());
		ab.create().show();
	}
	private String size(double a, int b)
	{
		if (a < 1024)
		{
			return String.format("%.2f%s", a, dw[b]);
		}
		b++;
		return size(a / 1024, b);
	}
	class handler extends Handler
	{

		@Override
		public void handleMessage(Message msg)
		{
			// TODO: Implement this method
			super.handleMessage(msg);
			if (msg.what == 0)
				adapter.notifyDataSetChanged();
			else if (msg.what == 1)
			{
				index++;
				if (index == lf.size())index = 0;
				play();
			}
			else if (msg.what == 2)
			{
				curtime.setText(gettime(a.getcur()));
				seek.setProgress((int)a.getcur());
			}
		}

	}
	public static String gettime(long i)
	{
		long m=i / 60;
		long s=i % 60;
		return String.format("%d:%02d", m, s);
	}
}
