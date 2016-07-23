package net.fcnav.plus.voice;

import java.io.File;

import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavSettings;
import net.fcnav.plus.ResourceManager;
import net.fcnav.plus.R;
import android.app.Activity;
import android.os.Build;

public class CommandPlayerFactory 
{
	public static CommandPlayer createCommandPlayer(String voiceProvider, FCNavApplication flcnavApplication, Activity ctx)
		throws CommandPlayerException
	{
		if (voiceProvider != null){
			FCNavSettings settings = flcnavApplication.getSettings();
			File parent = settings.extendFlcnavPath(ResourceManager.VOICE_PATH);
			File voiceDir = new File(parent, voiceProvider);
			if(!voiceDir.exists()){
				throw new CommandPlayerException(ctx.getString(R.string.voice_data_unavailable));
			}
			
			if (MediaCommandPlayerImpl.isMyData(voiceDir)) {
				return new MediaCommandPlayerImpl(flcnavApplication, settings, voiceProvider);
			} else if (Integer.parseInt(Build.VERSION.SDK) >= 4) {
				if (TTSCommandPlayerImpl.isMyData(voiceDir)) {
					return new TTSCommandPlayerImpl(ctx, settings, voiceProvider);
				}
			}
			throw new CommandPlayerException(ctx.getString(R.string.voice_data_not_supported));
		}
		return null;
	}
}
