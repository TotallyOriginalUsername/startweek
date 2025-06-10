tones = []
durations = []
toneCount = 0
previous_noTone = False
noTone_value = None

input_file = input("Song file: ")
output_file = input("Output file: ")
song_name = input("Song name: ").lower()
song_name_upper = song_name.upper()

with open(input_file, "r") as file:
    for line in file:
        line = line.replace(" ", "").strip()
        
        if "tone" in line:
            tones.append(line.split(",")[1][:-2])
            toneCount += 1
            previous_noTone = False
        elif "noTone" in line:
            noTone_value = line.split("(")[1][:-2]
            previous_noTone = True  # Set the flag for noTone
        elif "delay" in line:
            if previous_noTone and noTone_value is not None:
                tones.append(noTone_value)
                toneCount += 1
            durations.append(line.split("(")[1][:-2])
            previous_noTone = False

with open(output_file, 'w') as file:
    file.write(f"#define {song_name_upper}_NOTES {toneCount}\n")
    print(f"#define {song_name_upper}_NOTES {toneCount}")
    file.write(f"static struct note_duration {song_name}[{song_name_upper}_NOTES] = {{\n")
    print(f"static struct note_duration {song_name}[{song_name_upper}_NOTES] = {{")

    for x in range(toneCount):
        if x == toneCount - 1:
            file.write(f"    {{.note = {tones[x]}, .duration = {durations[x]}}}}};\n")
            print(f"    {{.note = {tones[x]}, .duration = {durations[x]}}}}};")
        else:
            file.write(f"    {{.note = {tones[x]}, .duration = {durations[x]}}},\n")
            print(f"    {{.note = {tones[x]}, .duration = {durations[x]}}},")