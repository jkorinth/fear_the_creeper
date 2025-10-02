from pydub import AudioSegment
import numpy as np

hiss = AudioSegment.from_file("creeper_hiss.mp3")
res = hiss.set_frame_rate(8000)

samples = np.array(res.get_array_of_samples()).astype(np.uint8)

with open("creeper_hiss.h", "w") as f:
    f.write("#pragma once\n")
    f.write("const uint8_t IGNITE[] PROGMEM = {\n");
    for i, sample in enumerate(samples[:8000]):
        if i % 10 == 0:
            f.write("   ")
        f.write(f"{sample}")
        if i < len(samples) - 1:
            f.write(", ")
        if (i+1) % 10 == 0:
            f.write("\n");
    f.write("};");
