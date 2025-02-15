#ifndef _MUSIC_MIDI_CTRL_API_H
#define _MUSIC_MIDI_CTRL_API_H

int music_midi_ctrl_pitch_bend(music_play_obj *hdl, u16 pitch_val, u8 chn);
int music_midi_ctrl_note_off(music_play_obj *hdl, u8 nkey, u8 chn);
int music_midi_ctrl_note_on(music_play_obj *hdl, u8 nkey, u8 nvel, u8 chn);
int music_midi_ctrl_set_prog(music_play_obj *hdl, u8 prog, u8 trk_num);
int music_midi_ctrl_config(music_play_obj *hdl, u32 cmd, void *parm);

#endif
