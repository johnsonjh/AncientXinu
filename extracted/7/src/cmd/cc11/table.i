L1:.ascii "jmp";.byte 0301;.ascii "\n"
.ascii "\0"
L2:.ascii "GB"
.ascii "jmp";.byte 0243;.ascii "(I)\n"
.ascii "\0"
L3:.ascii "jsr";.byte 0360;.ascii "c,MA\n"
.ascii "\0"
L4:.ascii "GB"
.ascii "jsr";.byte 0360;.ascii "c,#(I)\n"
.ascii "\0"
L5:.ascii "GA"
.ascii "jsr";.byte 0360;.ascii "c,(I)\n"
.ascii "\0"
L6:.ascii "clr";.byte 0311;.ascii "\n"
.ascii "\0"
L7:.ascii "clrf";.byte 0311;.ascii "\n"
.ascii "\0"
L8:.ascii "movC";.byte 0301;.ascii ",I\n"
.ascii "\0"
L9:.ascii "movof";.byte 0301;.ascii ",I\n"
.ascii "\0"
L10:.ascii "GB"
.ascii "movC";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L11:.ascii "GB"
.ascii "movof";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L12:.ascii "mov";.byte 0301;.ascii "+,I+\n"
.ascii "mov";.byte 0301;.ascii ",I\n"
.ascii "\0"
L13:.ascii "GB"
.ascii "mov";.byte 0243;.ascii "+2(I),I+\n"
.ascii "mov";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L14:.ascii "GA"
.ascii "\0"
L15:.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "M'C";.byte 0301;.ascii "''\n"
.ascii "\0"
L16:.ascii "mov";.byte 0301;.ascii "',I\n"
.ascii "M";.byte 0302;.ascii ",A''\n"
.ascii "\0"
L17:.ascii "GJ"
.ascii "movC";.byte 0243;.ascii "(J),I\n"
.ascii "M'C";.byte 0243;.ascii "(J)\n"
.ascii "\0"
L18:.ascii "GB"
.ascii "movC";.byte 0243;.ascii "(I),-(sp)\n"
.ascii "M'C";.byte 0243;.ascii "(I)\n"
.ascii "movC";.byte 0250;.ascii "sp)+,I\n"
.ascii "\0"
L19:.ascii "GJ"
.ascii "mov";.byte 0243;.ascii "(J),I\n"
.ascii "M";.byte 0302;.ascii ",#(J)\n"
.ascii "\0"
L20:.ascii "GB"
.ascii "mov";.byte 0243;.ascii "(I),-(sp)\n"
.ascii "M";.byte 0302;.ascii ",#(I)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "\0"
L21:.ascii "GA"
.ascii "M";.byte 0244;.ascii "1,A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
L22:.ascii "GJ"
.ascii "mov";.byte 0243;.ascii "+2(J),I+\n"
.ascii "mov";.byte 0243;.ascii "(J),I\n"
.ascii "M";.byte 0244;.ascii "1,#+2(J)\n"
.ascii "V";.byte 0243;.ascii "(J)\n"
.ascii "\0"
L23:.ascii "GB"
.ascii "mov";.byte 0243;.ascii "+2(I),-(sp)\n"
.ascii "mov";.byte 0243;.ascii "(I),-(sp)\n"
.ascii "M";.byte 0244;.ascii "1,#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I+\n"
.ascii "\0"
L24:.ascii "GA"
.ascii "MP";.byte 0311;.ascii "\n"
.ascii "\0"
L25:.ascii "GA"
.ascii "M";.byte 0311;.ascii "\n"
.ascii "M";.byte 0311;.ascii "+\n"
.ascii "V";.byte 0311;.ascii "\n"
.ascii "\0"
L26:.ascii "KA"
.ascii "movC";.byte 0311;.ascii ",A\n"
.ascii "\0"
L27:.ascii "KA"
.ascii "movfo";.byte 0311;.ascii ",A\n"
.ascii "\0"
L28:.ascii "GB"
.ascii "KA"
.ascii "movf";.byte 0311;.ascii ",#(I)\n"
.ascii "\0"
L29:.ascii "GB"
.ascii "movC";.byte 0302;.ascii ",#(I)\n"
.ascii "movC";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L30:.ascii "GB"
.ascii "KA"
.ascii "movfo";.byte 0311;.ascii ",#(I)\n"
.ascii "\0"
L31:.ascii "GB"
.ascii "KI"
.ascii "movC";.byte 0312;.ascii ",#(I)\n"
.ascii "movC";.byte 0312;.ascii ",I\n"
.ascii "\0"
L32:.ascii "KA"
.ascii "GJ"
.ascii "movf";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L33:.ascii "KA"
.ascii "GJ"
.ascii "movfo";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L34:.ascii "GD"
.ascii "KA"
.ascii "movC";.byte 0311;.ascii ",*(sp)+\n"
.ascii "\0"
L35:.ascii "GD"
.ascii "KA"
.ascii "movfo";.byte 0311;.ascii ",*(sp)+\n"
.ascii "\0"
L36:.ascii "KA"
.ascii "mov";.byte 0311;.ascii "+,A+\n"
.ascii "mov";.byte 0311;.ascii ",A\n"
.ascii "\0"
L37:.ascii "KA"
.ascii "GJ"
.ascii "mov";.byte 0311;.ascii "+,2+#(J)\n"
.ascii "mov";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L38:.ascii "GD"
.ascii "KA"
.ascii "mov";.byte 0311;.ascii ",*(sp)\n"
.ascii "add";.byte 0244;.ascii "2,(sp)\n"
.ascii "mov";.byte 0311;.ascii "+,*(sp)+\n"
.ascii "\0"
L39:.ascii "KA"
.ascii "bicC";.byte 0332;.ascii ",A'\n"
.ascii "bisC";.byte 0311;.ascii ",A''\n"
.ascii "\0"
L41:.ascii "KC"
.ascii "GB"
.ascii "bicC";.byte 0332;.ascii ",#(I)\n"
.ascii "bisC";.byte 0250;.ascii "sp),#(I)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "\0"
L42:.ascii "GA"
.ascii "\0"
L43:.ascii "GA"
.ascii "M'";.byte 0311;.ascii "\n"
.ascii "\0"
Add1:;L44:.ascii "GA"
.ascii "MD";.byte 0302;.ascii ",I\n"
.ascii "\0"
Add2:;L45:.ascii "GA"
.ascii "KJ"
.ascii "MD";.byte 0242;.ascii "(J),I\n"
.ascii "\0"
Add3:;L46:.ascii "GA"
.ascii "KI"
.ascii "MP";.byte 0312;.ascii ",I\n"
.ascii "\0"
Add4:;L47:.ascii "KD"
.ascii "GA"
.ascii "MD";.byte 0252;.ascii "(sp)+,I\n"
.ascii "\0"
Add5:;L48:.ascii "KC"
.ascii "GA"
.ascii "MP";.byte 0250;.ascii "sp)+,I\n"
.ascii "\0"
L49:.ascii "GA"
.ascii "M";.byte 0302;.ascii ",I+\n"
.ascii "V";.byte 0311;.ascii "\n"
.ascii "\0"
L50:.ascii "GA"
.ascii "KI"
.ascii "M";.byte 0312;.ascii ",I+\n"
.ascii "V";.byte 0311;.ascii "\n"
.ascii "\0"
L51:.ascii "GA"
.ascii "M";.byte 0302;.ascii ",I\n"
.ascii "M";.byte 0302;.ascii "+,I+\n"
.ascii "V";.byte 0311;.ascii "\n"
.ascii "\0"
Addl1:;L52:.ascii "GA"
.ascii "KI"
.ascii "M";.byte 0312;.ascii "+,I+\n"
.ascii "V";.byte 0311;.ascii "\n"
.ascii "M";.byte 0312;.ascii ",I\n"
.ascii "\0"
Addl2:;L53:.ascii "KC"
.ascii "GA"
.ascii "M";.byte 0250;.ascii "sp)+,I\n"
.ascii "M";.byte 0250;.ascii "sp)+,I+\n"
.ascii "V";.byte 0311;.ascii "\n"
.ascii "\0"
L55:.ascii "GC"
.ascii "KA"
.ascii "xor";.byte 0311;.ascii ",(sp)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "\0"
L57:.ascii "KC"
.ascii "GA"
.ascii "M";.byte 0311;.ascii ",(sp)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "M";.byte 0311;.ascii "+,(sp)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I+\n"
.ascii "\0"
L58:.ascii "GA"
.ascii "asr";.byte 0311;.ascii "\n"
.ascii "\0"
L63:.ascii "GA"
.ascii "T"
.ascii "M";.byte 0302;.ascii ",I-\n"
.ascii "\0"
L64:.ascii "GA"
.ascii "T"
.ascii "KJ"
.ascii "M";.byte 0242;.ascii "(J),I-\n"
.ascii "\0"
L65:.ascii "GA"
.ascii "T"
.ascii "KI"
.ascii "M";.byte 0312;.ascii ",I-\n"
.ascii "\0"
L66:.ascii "KC"
.ascii "GA"
.ascii "T"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "\0"
Addq1:;L71:.ascii "M";.byte 0302;.ascii ",A'\n"
.ascii "mov";.byte 0301;.ascii "'',I\n"
.ascii "\0"
Addq1a:;L72:.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "MP";.byte 0302;.ascii ",I\n"
.ascii "movC";.byte 0311;.ascii ",A''\n"
.ascii "\0"
Addq2:;L73:.ascii "KB"
.ascii "M";.byte 0242;.ascii "(I),A'\n"
.ascii "mov";.byte 0301;.ascii "'',I\n"
.ascii "\0"
Addq3:;L74:.ascii "KA"
.ascii "M";.byte 0311;.ascii ",A'\n"
.ascii "mov";.byte 0301;.ascii "'',I\n"
.ascii "\0"
Addq4:;L75:.ascii "KB"
.ascii "GJ"
.ascii "M";.byte 0242;.ascii "(I),#(J)\n"
.ascii "mov";.byte 0243;.ascii "(J),I\n"
.ascii "\0"
Addq4a:;L76:.ascii "movf";.byte 0301;.ascii "',I\n"
.ascii "KI"
.ascii "MP";.byte 0312;.ascii ",I\n"
.ascii "movf";.byte 0311;.ascii ",A''\n"
.ascii "\0"
Addq5:;L77:.ascii "KC"
.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "MP";.byte 0250;.ascii "sp)+,I\n"
.ascii "movC";.byte 0311;.ascii ",A''\n"
.ascii "\0"
Addq6:;L78:.ascii "KC"
.ascii "movof";.byte 0301;.ascii "',I\n"
.ascii "MP";.byte 0250;.ascii "sp)+,I\n"
.ascii "movfo";.byte 0311;.ascii ",A''\n"
.ascii "\0"
Addq7:;L79:.ascii "KA"
.ascii "GJ"
.ascii "M";.byte 0311;.ascii ",#(J)\n"
.ascii "mov";.byte 0243;.ascii "(J),I\n"
.ascii "\0"
Addq8:;L80:.ascii "KC"
.ascii "GB"
.ascii "M";.byte 0250;.ascii "sp)+,#(I)\n"
.ascii "mov";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
Addq9:;L81:.ascii "GD"
.ascii "KC"
.ascii "movC";.byte 0252;.ascii "2(sp),I\n"
.ascii "MP";.byte 0250;.ascii "sp)+,I\n"
.ascii "movC";.byte 0311;.ascii ",*(sp)+\n"
.ascii "\0"
Addq9a:;L82:.ascii "KC"
.ascii "GB"
.ascii "movC";.byte 0243;.ascii "(I),I\n"
.ascii "MP";.byte 0250;.ascii "sp)+,I\n"
.ascii "movC";.byte 0311;.ascii ",#(I)\n"
.ascii "\0"
Addq10:;L83:.ascii "KC"
.ascii "GB"
.ascii "movof";.byte 0243;.ascii "(I),J\n"
.ascii "MP";.byte 0250;.ascii "sp)+,J\n"
.ascii "movfo";.byte 0312;.ascii ",#(I)\n"
.ascii "movf";.byte 0312;.ascii ",I\n"
.ascii "\0"
L84:.ascii "M";.byte 0302;.ascii ",A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "GA"
.ascii "\0"
L85:.ascii "M";.byte 0302;.ascii "+,A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "M";.byte 0302;.ascii ",A\n"
.ascii "GA"
.ascii "\0"
L86:.ascii "KA"
.ascii "M";.byte 0311;.ascii "+,A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "M";.byte 0311;.ascii ",A\n"
.ascii "GA"
.ascii "\0"
L87:.ascii "GB"
.ascii "M";.byte 0302;.ascii ",#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "mov";.byte 0243;.ascii "+2(I),I+\n"
.ascii "mov";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L88:.ascii "GB"
.ascii "M";.byte 0302;.ascii "+,#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "M";.byte 0302;.ascii ",#(I)\n"
.ascii "mov";.byte 0243;.ascii "+2(I),I+\n"
.ascii "mov";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L89:.ascii "KC"
.ascii "GB"
.ascii "M";.byte 0250;.ascii "sp)+,#(I)\n"
.ascii "M";.byte 0250;.ascii "sp)+,#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "mov";.byte 0243;.ascii "+2(I),I+\n"
.ascii "mov";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L97:.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "MP";.byte 0302;.ascii ",I-\n"
.ascii "movC";.byte 0311;.ascii "-,A''\n"
.ascii "\0"
L98:.ascii "KC"
.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "movC";.byte 0311;.ascii "-,A''\n"
.ascii "\0"
L99:.ascii "KC"
.ascii "GJ"
.ascii "movC";.byte 0243;.ascii "(J),I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "movC";.byte 0311;.ascii "-,#(J)\n"
.ascii "\0"
L100:.ascii "GD"
.ascii "KC"
.ascii "movC";.byte 0252;.ascii "2(sp),I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "movC";.byte 0311;.ascii "-,*(sp)+\n"
.ascii "\0"
L107:.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0302;.ascii ",I-\n"
.ascii "movC";.byte 0311;.ascii ",A''\n"
.ascii "\0"
L108:.ascii "KC"
.ascii "movC";.byte 0301;.ascii "',I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "movC";.byte 0311;.ascii ",A''\n"
.ascii "\0"
L109:.ascii "KC"
.ascii "GJ"
.ascii "movC";.byte 0243;.ascii "(J),I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "movC";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L110:.ascii "GD"
.ascii "KC"
.ascii "movC";.byte 0252;.ascii "2(sp),I\n"
.ascii "V";.byte 0311;.ascii "-\n"
.ascii "M";.byte 0250;.ascii "sp)+,I-\n"
.ascii "mov";.byte 0311;.ascii ",*(sp)+\n"
.ascii "\0"
L112:.ascii "KC"
.ascii "movb";.byte 0301;.ascii "',I\n"
.ascii "xor";.byte 0311;.ascii ",(sp)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "movb";.byte 0311;.ascii ",A''\n"
.ascii "\0"
L113:.ascii "GD"
.ascii "movC";.byte 0252;.ascii "(sp),-(sp)\n"
.ascii "KA"
.ascii "xor";.byte 0311;.ascii ",(sp)\n"
.ascii "movC";.byte 0250;.ascii "sp)+,I\n"
.ascii "movC";.byte 0311;.ascii ",*(sp)+\n"
.ascii "\0"
L114:.ascii "asrC";.byte 0301;.ascii "'\n"
.ascii "movC";.byte 0301;.ascii "'',I\n"
.ascii "\0"
L115:.ascii "GB"
.ascii "asrC";.byte 0243;.ascii "(I)\n"
.ascii "movC";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L121:.ascii "movif";.byte 0301;.ascii ",I\n"
.ascii "\0"
L122:.ascii "GB"
.ascii "movif";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L123:.ascii "GA"
.ascii "movif";.byte 0311;.ascii ",I\n"
.ascii "\0"
L124:.ascii "GA"
.ascii "movfi";.byte 0311;.ascii ",I\n"
.ascii "\0"
L125:.ascii "GA"
.ascii "setl\n"
.ascii "movfi";.byte 0311;.ascii ",-(sp)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I+\n"
.ascii "seti\n"
.ascii "\0"
L126:.ascii "setl\n"
.ascii "movif";.byte 0301;.ascii ",I\n"
.ascii "seti\n"
.ascii "\0"
L127:.ascii "GB"
.ascii "setl\n"
.ascii "movif";.byte 0243;.ascii "(I),I\n"
.ascii "seti\n"
.ascii "\0"
L128:.ascii "GC"
.ascii "setl\n"
.ascii "movif";.byte 0250;.ascii "sp)+,I\n"
.ascii "seti\n"
.ascii "\0"
L129:.ascii "GI!"
.ascii "clr";.byte 0311;.ascii "\n"
.ascii "\0"
L130:.ascii "GA"
.ascii "mov";.byte 0311;.ascii ",J\n"
.ascii "clr";.byte 0311;.ascii "\n"
.ascii "\0"
L131:.ascii "GI!"
.ascii "sxt";.byte 0311;.ascii "\n"
.ascii "\0"
L132:.ascii "GA"
.ascii "mov";.byte 0311;.ascii ",J\n"
.ascii "sxt";.byte 0311;.ascii "\n"
.ascii "\0"
L133:.ascii "mov";.byte 0301;.ascii "+,I\n"
.ascii "\0"
L134:.ascii "GB"
.ascii "mov";.byte 0243;.ascii "+2(I),I\n"
.ascii "\0"
L135:.ascii "KC"
.ascii "GC"
.ascii "jsr";.byte 0360;.ascii "c,M\n"
.ascii "add";.byte 0244;.ascii "10,sp\n"
.ascii "\0"
L136:.ascii "KC"
.ascii "GC"
.ascii "jsr";.byte 0360;.ascii "c,M\n"
.ascii "add";.byte 0244;.ascii "6,sp\n"
.ascii "\0"
L137:.ascii "GA"
.ascii "movb";.byte 0311;.ascii ",I\n"
.ascii "\0"
move1:;L138:.ascii "M'C";.byte 0301;.ascii "\n"
.ascii "\0"
move2:;L139:.ascii "GB"
.ascii "M'C";.byte 0243;.ascii "(I)\n"
.ascii "\0"
move3:;L140:.ascii "ML";.byte 0302;.ascii ",A\n"
.ascii "\0"
move4:;L141:.ascii "KB"
.ascii "ML";.byte 0242;.ascii "(I),A\n"
.ascii "\0"
move5:;L142:.ascii "KA"
.ascii "MC";.byte 0311;.ascii ",A\n"
.ascii "\0"
move6:;L143:.ascii "GB"
.ascii "ML";.byte 0302;.ascii ",#(I)\n"
.ascii "\0"
move7:;L144:.ascii "GB"
.ascii "KJ"
.ascii "ML";.byte 0242;.ascii "(J),#(I)\n"
.ascii "\0"
move8:;L145:.ascii "GB"
.ascii "KI"
.ascii "MC";.byte 0312;.ascii ",#(I)\n"
.ascii "\0"
move9:;L146:.ascii "KB"
.ascii "GJ"
.ascii "ML";.byte 0242;.ascii "(I),#(J)\n"
.ascii "\0"
move10:;L147:.ascii "KA"
.ascii "GJ"
.ascii "MC";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
move11:;L148:.ascii "GD"
.ascii "KB"
.ascii "ML";.byte 0242;.ascii "(I),*(sp)+\n"
.ascii "\0"
move12:;L149:.ascii "GD"
.ascii "KA"
.ascii "MC";.byte 0311;.ascii ",*(sp)+\n"
.ascii "\0"
L150:.ascii "KA"
.ascii "movfi";.byte 0311;.ascii ",A\n"
.ascii "\0"
L151:.ascii "KA"
.ascii "GJ"
.ascii "movfi";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L152:.ascii "clr";.byte 0301;.ascii "\n"
.ascii "clr";.byte 0301;.ascii "+\n"
.ascii "\0"
L153:.ascii "GB"
.ascii "clr";.byte 0243;.ascii "(I)\n"
.ascii "clr";.byte 0262;.ascii "+#(I)\n"
.ascii "\0"
move13a:;L154:.ascii "M";.byte 0302;.ascii ",A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
L155:.ascii "KB"
.ascii "mov";.byte 0242;.ascii "(I),A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
L156:.ascii "KA"
.ascii "mov";.byte 0311;.ascii ",A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
L157:.ascii "KA"
.ascii "setl\n"
.ascii "movfi";.byte 0311;.ascii ",A\n"
.ascii "seti\n"
.ascii "\0"
L158:.ascii "KA"
.ascii "GJ"
.ascii "setl\n"
.ascii "movfi";.byte 0311;.ascii ",#(J)\n"
.ascii "seti\n"
.ascii "\0"
move13:;L159:.ascii "M";.byte 0302;.ascii ",A\n"
.ascii "M";.byte 0302;.ascii "+,A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
move14:;L160:.ascii "KB"
.ascii "M";.byte 0242;.ascii "(I),A\n"
.ascii "M";.byte 0242;.ascii "+2(I),A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
move15:;L161:.ascii "KA"
.ascii "M";.byte 0311;.ascii ",A\n"
.ascii "M";.byte 0311;.ascii "+,A+\n"
.ascii "V";.byte 0301;.ascii "\n"
.ascii "\0"
move14a:;L162:.ascii "GB"
.ascii "M";.byte 0302;.ascii ",#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "\0"
move16a:;L163:.ascii "GB"
.ascii "M";.byte 0302;.ascii "+,#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "M";.byte 0302;.ascii ",#(I)\n"
.ascii "\0"
move16:;L164:.ascii "KA"
.ascii "GJ"
.ascii "M";.byte 0311;.ascii "+,#+2(J)\n"
.ascii "V";.byte 0243;.ascii "(J)\n"
.ascii "M";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L165:.ascii "KC"
.ascii "GB"
.ascii "mov";.byte 0250;.ascii "sp)+,#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "\0"
move17:;L166:.ascii "KC"
.ascii "GB"
.ascii "M";.byte 0250;.ascii "sp)+,#(I)\n"
.ascii "M";.byte 0250;.ascii "sp)+,#+2(I)\n"
.ascii "V";.byte 0243;.ascii "(I)\n"
.ascii "\0"
L186:.ascii "GD"
.ascii "KA"
.ascii "M";.byte 0311;.ascii ",*(sp)\n"
.ascii "mov";.byte 0250;.ascii "sp)+,I\n"
.ascii "M";.byte 0311;.ascii "+,2(I)\n"
.ascii "\0"
L187:.ascii "\0"
L188:.ascii "M'C";.byte 0301;.ascii "\n"
.ascii "\0"
L194:.ascii "KB"
.ascii "movC";.byte 0301;.ascii "',J\n"
.ascii "M";.byte 0242;.ascii "(I),J\n"
.ascii "movC";.byte 0312;.ascii ",A''\n"
.ascii "\0"
L195:.ascii "KA"
.ascii "movC";.byte 0301;.ascii "',J\n"
.ascii "M";.byte 0311;.ascii ",J\n"
.ascii "movC";.byte 0312;.ascii ",A''\n"
.ascii "\0"
L198:.ascii "KC"
.ascii "GB"
.ascii "movC";.byte 0243;.ascii "(I),J\n"
.ascii "M";.byte 0250;.ascii "sp)+,J\n"
.ascii "movC";.byte 0312;.ascii ",#(I)\n"
.ascii "\0"
L207:.ascii "asrC";.byte 0301;.ascii "\n"
.ascii "\0"
L208:.ascii "GB"
.ascii "asrC";.byte 0243;.ascii "(I)\n"
.ascii "\0"
L209:.ascii "aslC";.byte 0301;.ascii "\n"
.ascii "\0"
L210:.ascii "GB"
.ascii "aslC";.byte 0243;.ascii "(I)\n"
.ascii "\0"
L211:.ascii "ash";.byte 0302;.ascii ",A\n"
.ascii "\0"
L212:.ascii "KB"
.ascii "ash";.byte 0242;.ascii "(I),A\n"
.ascii "\0"
L213:.ascii "KA"
.ascii "ash";.byte 0311;.ascii ",A\n"
.ascii "\0"
L214:.ascii "GA"
.ascii "ashc";.byte 0302;.ascii ",I\n"
.ascii "mov";.byte 0311;.ascii ",A\n"
.ascii "mov";.byte 0311;.ascii "+,A+\n"
.ascii "\0"
L215:.ascii "KC"
.ascii "GA"
.ascii "ashc";.byte 0250;.ascii "sp)+,I\n"
.ascii "mov";.byte 0311;.ascii ",A\n"
.ascii "mov";.byte 0311;.ascii "+,A+\n"
.ascii "\0"
L216:.ascii "GD"
.ascii "KC"
.ascii "mov";.byte 0262;.ascii "(sp),I\n"
.ascii "mov";.byte 0262;.ascii "(I),I+\n"
.ascii "mov";.byte 0250;.ascii "I),I\n"
.ascii "ashc";.byte 0250;.ascii "sp)+,I\n"
.ascii "mov";.byte 0311;.ascii ",*(sp)\n"
.ascii "add";.byte 0244;.ascii "2,(sp)\n"
.ascii "mov";.byte 0311;.ascii "+,*(sp)+\n"
.ascii "\0"
L217:.ascii "bicC";.byte 0332;.ascii ",A'\n"
.ascii "bisC";.byte 0302;.ascii ",A''\n"
.ascii "\0"
L218:.ascii "KA"
.ascii "bicC";.byte 0332;.ascii ",A'\n"
.ascii "bisC";.byte 0311;.ascii ",A''\n"
.ascii "\0"
L219:.ascii "GB"
.ascii "bicC";.byte 0332;.ascii ",#(I)\n"
.ascii "bisC";.byte 0302;.ascii ",#(I)\n"
.ascii "\0"
fas1:;L220:.ascii "KA"
.ascii "GJ"
.ascii "bicC";.byte 0332;.ascii ",#(J)\n"
.ascii "bisC";.byte 0311;.ascii ",#(J)\n"
.ascii "\0"
L221:.ascii "GB"
.ascii "KI"
.ascii "bicC";.byte 0332;.ascii ",#(I)\n"
.ascii "bisC";.byte 0312;.ascii ",#(I)\n"
.ascii "\0"
L222:.ascii "KC"
.ascii "GB"
.ascii "bicC";.byte 0332;.ascii ",#(I)\n"
.ascii "bisC";.byte 0250;.ascii "sp)+,#(I)\n"
.ascii "\0"
L224:.ascii "movof";.byte 0301;.ascii ",I\n"
.ascii "\0"
L226:.ascii "GB"
.ascii "movof";.byte 0243;.ascii "(I),I\n"
.ascii "\0"
L227:.ascii "GE"
.ascii "\0"
L238:.ascii "tst";.byte 0301;.ascii "\n"
.ascii "X0"
.ascii "tst";.byte 0301;.ascii "+\n"
.ascii "X1"
.ascii "\0"
L239:.ascii "tst";.byte 0301;.ascii "\n"
.ascii "X0"
.ascii "cmp";.byte 0301;.ascii "+,B\n"
.ascii "X1"
.ascii "\0"
lcmp1:;L240:.ascii "M";.byte 0301;.ascii ",B\n"
.ascii "X0"
.ascii "M";.byte 0301;.ascii "+,B+\n"
.ascii "X1"
.ascii "\0"
L241:.ascii "GB"
.ascii "tst";.byte 0243;.ascii "(I)\n"
.ascii "X0"
.ascii "tst";.byte 0243;.ascii "+2(I)\n"
.ascii "X1"
.ascii "\0"
L242:.ascii "GB"
.ascii "tst";.byte 0243;.ascii "(I)\n"
.ascii "X0"
.ascii "cmp";.byte 0243;.ascii "+2(I),B\n"
.ascii "X1"
.ascii "\0"
lcmp2:;L243:.ascii "GB"
.ascii "M";.byte 0243;.ascii "(I),B\n"
.ascii "X0"
.ascii "M";.byte 0243;.ascii "+2(I),B+\n"
.ascii "X1"
.ascii "\0"
L244:.ascii "GA"
.ascii "tst";.byte 0311;.ascii "\n"
.ascii "X0"
.ascii "tst";.byte 0311;.ascii "+\n"
.ascii "X1"
.ascii "\0"
L245:.ascii "GA"
.ascii "tst";.byte 0311;.ascii "\n"
.ascii "X0"
.ascii "cmp";.byte 0311;.ascii "+,B\n"
.ascii "X1"
.ascii "\0"
lcmp3:;L246:.ascii "GA"
.ascii "M";.byte 0311;.ascii ",B\n"
.ascii "X0"
.ascii "M";.byte 0311;.ascii "+,B+\n"
.ascii "X1"
.ascii "\0"
lcmp4:;L247:.ascii "GB"
.ascii "KJ"
.ascii "M";.byte 0243;.ascii "(I),\"(J)\n"
.ascii "X0"
.ascii "M";.byte 0243;.ascii "+2(I),\"+2(J)\n"
.ascii "X1"
.ascii "\0"
lcmp5:;L248:.ascii "GA"
.ascii "KJ"
.ascii "M";.byte 0311;.ascii ",\"(J)\n"
.ascii "X0"
.ascii "M";.byte 0311;.ascii "+,\"+2(J)\n"
.ascii "X1"
.ascii "\0"
lcmp6:;L249:.ascii "GC"
.ascii "KA"
.ascii "mov";.byte 0311;.ascii ",-(sp)\n"
.ascii "mov";.byte 0264;.ascii "(sp),I\n"
.ascii "mov";.byte 0250;.ascii "sp)+,2(sp)\n"
.ascii "M";.byte 0250;.ascii "sp)+,(sp)+\n"
.ascii "X0"
.ascii "M";.byte 0311;.ascii ",I+\n"
.ascii "X1"
.ascii "\0"
L255:.ascii "/bit";.byte 0244;.ascii "0,A\n"
.ascii "/X0"
.ascii "bit";.byte 0302;.ascii ",A+\n"
.ascii "X1"
.ascii "\0"
L256:.ascii "GB"
.ascii "/bit";.byte 0244;.ascii "0,\"(I)\n"
.ascii "/X0"
.ascii "bit";.byte 0302;.ascii ",\"+2(I)\n"
.ascii "X1"
.ascii "\0"
L263:.ascii "GA"
.ascii "/bit";.byte 0244;.ascii "0,I\n"
.ascii "/X0"
.ascii "bit";.byte 0302;.ascii ",I+\n"
.ascii "X1"
.ascii "\0"
L264:.ascii "HA"
.ascii "\0"
L265:.ascii "clrC";.byte 0255;.ascii "(sp)\n"
.ascii "\0"
L266:.ascii "mov";.byte 0301;.ascii ",-(sp)\n"
.ascii "\0"
L267:.ascii "GB"
.ascii "mov";.byte 0243;.ascii "(I),-(sp)\n"
.ascii "\0"
L268:.ascii "mov";.byte 0301;.ascii "+,-(sp)\n"
.ascii "mov";.byte 0301;.ascii ",-(sp)\n"
.ascii "\0"
L269:.ascii "GC"
.ascii "M'";.byte 0250;.ascii "sp)\n"
.ascii "\0"
L270:.ascii "GC"
.ascii "M";.byte 0302;.ascii ",(sp)\n"
.ascii "\0"
L271:.ascii "GC"
.ascii "KB"
.ascii "M";.byte 0242;.ascii "(I),(sp)\n"
.ascii "\0"
L272:.ascii "GC"
.ascii "KA"
.ascii "M";.byte 0311;.ascii ",(sp)\n"
.ascii "\0"
L273:.ascii "GC"
.ascii "clr";.byte 0255;.ascii "(sp)\n"
.ascii "\0"
L274:.ascii "GC"
.ascii "sxt";.byte 0255;.ascii "(sp)\n"
.ascii "\0"
L275:.ascii "GA"
.ascii "setl\n"
.ascii "movfi";.byte 0311;.ascii ",-(sp)\n"
.ascii "seti\n"
.ascii "\0"
L276:.ascii "GA!"
.ascii "KI!"
.ascii "\0"
L277:.ascii "KC"
.ascii "GA!"
.ascii "mov";.byte 0250;.ascii "sp)+,r1\n"
.ascii "\0"
.align 1
 #
 # c code tables-- compile to register
 #

.globl	_regtab

.data
_regtab:
.int	106,	cr106
.int	30,	cr70
.int	31,	cr70
.int	32,	cr32
.int	33,	cr32
.int	37,	cr37
.int	38,	cr37
.int	98,	cr100
.int	99,	cr100
.int	80,	cr80
.int	40,	cr40
 # - like +
.int	41,	cr40
.int	42,	cr42
.int	43,	cr43
.int	14,	cr43
.int	44,	cr43
.int	45,	cr45
.int	46,	cr40
.int	55, 	cr40
.int	48,	cr40
.int	49,	cr49
.int	70,	cr70
.int	71,	cr70
.int	72,	cr72
.int	73,	cr73
.int	74,	cr74
.int	75,	cr75
.int	76,	cr72
.int	78,	cr70
.int	85,	cr70
.int	79,	cr79
.int	102,	cr102
.int	51,	cr51
.int	52,	cr52
.int	56,	cr56
.int	57,	cr57
.int	58,	cr58
.int	59,	cr59
.int	91,	cr91
.int	82,	cr82
.int	83,	cr82
.int	84,	cr82
.int	86,	cr86
.int	87,	cr86
.int	88,	cr86
.int	16,	cr16
.int	92,	cr92
.int	17,	cr43
.int	18,	cr74
.int	109, cr109
.int	0
.text

 # goto
.int 0; cr102:
.byte 020,00;.byte 077,00
.int L1
.byte 0177,00;.byte 077,00
.int L2
 # call
.int 0; cr100:
.byte 020,00;.byte 077,00
.int L3
.byte 0177,00;.byte 077,00
.int L4
.byte 077,00;.byte 077,00
.int L5
 # addressible
.int 0; cr106:
.byte 04,00;.byte 077,00
.int L6
.byte 04,04;.byte 077,00
.int L7
.byte 020,00;.byte 077,00
.int L8
.byte 020,05;.byte 077,00
.int L8
.byte 020,04;.byte 077,00
.int L9
.byte 0177,00;.byte 077,00
.int L10
.byte 0177,05;.byte 077,00
.int L10
.byte 0177,04;.byte 077,00
.int L11
.byte 020,010;.byte 077,00
.int L12
.byte 0177,010;.byte 077,00
.int L13
.byte 077,00;.byte 077,00
.int L14
 # ++,-- postfix
.int 0; cr32:
.byte 020,00;.byte 05,00
.int L15
.byte 020,01;.byte 077,00
.int L16
.byte 0124,00;.byte 05,00
.int L17
.byte 0177,00;.byte 05,00
.int L18
.byte 0124,01;.byte 077,00
.int L19
.byte 0177,01;.byte 077,00
.int L20
.byte 020,010;.byte 05,00
.int L21
.byte 0124,010;.byte 05,00
.int L22
.byte 0177,010;.byte 05,00
.int L23
 # - unary, ~
.int 0; cr37:
.byte 077,00;.byte 077,00
.int L24
.byte 077,04;.byte 077,00
.int L24
.byte 077,010;.byte 077,00
.int L25
 # =
.int 0; cr80:
.byte 020,00;.byte 077,00
.int L26
.byte 020,05;.byte 077,04
.int L26
.byte 020,04;.byte 077,04
.int L27
.byte 0177,05;.byte 020,04
.int L28
.byte 0177,00;.byte 020,01
.int L29
.byte 0177,04;.byte 020,04
.int L30
.byte 0177,00;.byte 024,00
.int L31
.byte 0124,05;.byte 077,04
.int L32
.byte 0124,04;.byte 077,04
.int L33
.byte 0177,00;.byte 077,00
.int L34
.byte 0177,05;.byte 077,04
.int L34
.byte 0177,04;.byte 077,04
.int L35
.byte 020,010;.byte 077,010
.int L36
.byte 0124,010;.byte 077,010
.int L37
.byte 0177,010;.byte 077,010
.int L38
 # field assign, value in reg.
.int 0; cr16:
.byte 020,00;.byte 077,00
.int L39
.byte 0124,00;.byte 077,00
.int L40
.set L40,fas1

.byte 0177,00;.byte 077,00
.int L41
 # +, -, |, &~, <<
.int 0; cr40:
.byte 077,00;.byte 04,00
.int L42
.byte 077,00;.byte 05,00
.int L43
.byte 077,00;.byte 020,01
.int L44
.byte 077,04;.byte 020,05
.int L44
.byte 077,00;.byte 0124,01
.int L45
.byte 077,04;.byte 0124,05
.int L45
.byte 077,00;.byte 024,00
.int L46
.byte 077,04;.byte 024,04
.int L46
.byte 077,00;.byte 0177,01
.int L47
.byte 077,04;.byte 0177,05
.int L47
.byte 077,00;.byte 077,00
.int L48
.byte 077,04;.byte 077,04
.int L48
.byte 077,010;.byte 010,00
.int L49
.byte 077,010;.byte 020,011
.int L49
.byte 077,010;.byte 024,011
.int L50
.byte 077,010;.byte 020,010
.int L51
.byte 077,010;.byte 024,010
.int L52
.byte 077,010;.byte 077,010
.int L53
 # ^ -- xor
.int 0; cr49:
.byte 077,00;.byte 024,00
.int L54
.set L54,Add3

.byte 077,00;.byte 077,00
.int L55
.byte 077,010;.byte 024,010
.int L56
.set L56,Addl1

.byte 077,010;.byte 077,010
.int L57
 # >> (all complicated cases taken care of by << -)
.int 0; cr45:
.byte 077,00;.byte 05,00
.int L58
 # * -- I must be odd on integers
.int 0; cr42:
.byte 077,00;.byte 020,01
.int L59
.byte 077,04;.byte 020,05
.int L59
.set L59,Add1

.byte 077,00;.byte 0124,01
.int L60
.byte 077,04;.byte 0124,05
.int L60
.set L60,Add2

.byte 077,00;.byte 024,00
.int L61
.byte 077,04;.byte 024,04
.int L61
.set L61,Add3

.byte 077,00;.byte 077,00
.int L62
.byte 077,04;.byte 077,04
.int L62
.set L62,Add5

 # / I must be odd on integers
.int 0; cr43:
.byte 077,00;.byte 020,01
.int L63
.byte 077,00;.byte 0124,01
.int L64
.byte 077,00;.byte 024,00
.int L65
.byte 077,00;.byte 077,00
.int L66
.byte 077,04;.byte 020,05
.int L67
.set L67,Add1

.byte 077,04;.byte 0124,05
.int L68
.set L68,Add2

.byte 077,04;.byte 024,04
.int L69
.set L69,Add3

.byte 077,04;.byte 077,04
.int L70
.set L70,Add5

 # =+, =-, =|, =&~
.int 0; cr70:
.byte 020,01;.byte 020,01
.int L71
.byte 020,00;.byte 020,01
.int L72
.byte 020,05;.byte 020,05
.int L72
.byte 020,01;.byte 0177,01
.int L73
.byte 020,01;.byte 077,00
.int L74
.byte 0124,01;.byte 0177,01
.int L75
.byte 020,05;.byte 024,04
.int L76
.byte 020,00;.byte 077,00
.int L77
.byte 020,05;.byte 077,04
.int L77
.byte 020,04;.byte 077,04
.int L78
.byte 0124,01;.byte 077,00
.int L79
.byte 0177,01;.byte 077,00
.int L80
.byte 0177,00;.byte 077,00
.int L81
.byte 0177,05;.byte 077,04
.int L82
.byte 0177,04;.byte 077,04
.int L83
.byte 020,010;.byte 010,00
.int L84
.byte 020,010;.byte 020,010
.int L85
.byte 020,010;.byte 077,010
.int L86
.byte 0177,010;.byte 010,00
.int L87
.byte 0177,010;.byte 020,010
.int L88
.byte 0177,010;.byte 077,010
.int L89
 # =*, =<< (for integer multiply, I must be odd)
.int 0; cr72:
.byte 020,00;.byte 020,01
.int L90
.byte 020,05;.byte 020,05
.int L90
.set L90,Addq1a

.byte 020,05;.byte 024,04
.int L91
.set L91,Addq4a

.byte 020,00;.byte 077,00
.int L92
.byte 020,05;.byte 077,04
.int L92
.set L92,Addq5

.byte 020,04;.byte 077,04
.int L93
.set L93,Addq6

.byte 0177,00;.byte 077,00
.int L94
.set L94,Addq9

.byte 0177,05;.byte 077,04
.int L95
.set L95,Addq9a

.byte 0177,04;.byte 077,04
.int L96
.set L96,Addq10

 # =/ ;  I must be odd on integers
.int 0; cr73:
.byte 020,00;.byte 020,01
.int L97
.byte 020,00;.byte 077,00
.int L98
.byte 0124,00;.byte 077,00
.int L99
.byte 0177,00;.byte 077,00
.int L100
.byte 020,05;.byte 020,05
.int L101
.set L101,Addq1a

.byte 020,05;.byte 024,04
.int L102
.set L102,Addq4a

.byte 020,05;.byte 077,04
.int L103
.set L103,Addq5

.byte 020,04;.byte 077,04
.int L104
.set L104,Addq6

.byte 0177,05;.byte 077,04
.int L105
.set L105,Addq9a

.byte 0177,04;.byte 077,04
.int L106
.set L106,Addq10

 # =mod; I must be odd on integers
.int 0; cr74:
.byte 020,00;.byte 020,01
.int L107
.byte 020,00;.byte 077,00
.int L108
.byte 0124,00;.byte 077,00
.int L109
.byte 0177,00;.byte 077,00
.int L110
 # =^ -- =xor
.int 0; cr79:
.byte 020,01;.byte 077,00
.int L111
.set L111,Addq3

.byte 020,03;.byte 077,00
.int L112
.byte 0177,00;.byte 077,00
.int L113
 # =>> (all complicated cases done by =<< -)
.int 0; cr75:
.byte 020,00;.byte 05,00
.int L114
.byte 0177,00;.byte 05,00
.int L115
 # << for longs
.int 0; cr91:
.byte 077,010;.byte 020,01
.int L116
.set L116,Add1

.byte 077,010;.byte 0124,01
.int L117
.set L117,Add2

.byte 077,010;.byte 024,00
.int L118
.set L118,Add3

.byte 077,010;.byte 0177,01
.int L119
.set L119,Add4

.byte 077,010;.byte 077,00
.int L120
.set L120,Add5

 # int -> float
.int 0; cr51:
.byte 020,01;.byte 077,00
.int L121
.byte 0177,01;.byte 077,00
.int L122
.byte 077,00;.byte 077,00
.int L123
 # float, double -> int
.int 0; cr52:
.byte 077,04;.byte 077,00
.int L124
 # double (float) to long
.int 0; cr56:
.byte 077,04;.byte 077,00
.int L125
 # long to double
.int 0; cr57:
.byte 020,010;.byte 077,00
.int L126
.byte 0177,010;.byte 077,00
.int L127
.byte 077,010;.byte 077,00
.int L128
 # integer to long
.int 0; cr58:
.byte 024,011;.byte 077,00
.int L129
.byte 077,011;.byte 077,00
.int L130
.byte 024,00;.byte 077,00
.int L131
.byte 077,00;.byte 077,00
.int L132
 # long to integer
.int 0; cr59:
.byte 020,010;.byte 077,00
.int L133
.byte 0177,010;.byte 077,00
.int L134
 # *, /, remainder for longs.
.int 0; cr82:
.byte 077,010;.byte 077,010
.int L135
 # =*, =/, =rem for longs
 # Operands of the form &x op y, so stack space is known.
.int 0; cr86:
.byte 077,00;.byte 077,010
.int L136
 # convert integer to character (sign extend)
.int 0; cr109:
.byte 077,00;.byte 077,00
.int L137
 #
 # c code tables -- compile for side effects.
 # Olso set condition codes properly (except for ++, --)
 #

.globl	_efftab

.data
_efftab:
.int	30,	ci70
.int	31,	ci70
.int	32,	ci70
.int	33,	ci70
.int	80,	ci80
.int	70,	ci70
 # - like +
.int	71,	ci70	
.int	78,	ci78
.int	79,	ci79
.int	85,	ci78
.int	75,	ci75
.int	76,	ci76
.int	16,	ci16
.int	116,	ci116
.int	0
.text

 # =
.int 0; ci80:
.byte 020,00;.byte 04,00
.int L138
.byte 020,05;.byte 04,04
.int L138
.byte 0177,00;.byte 04,00
.int L139
.byte 0177,05;.byte 04,04
.int L139
.byte 020,00;.byte 020,01
.int L140
.byte 020,03;.byte 020,00
.int L140
.byte 020,03;.byte 0177,00
.int L141
.byte 020,00;.byte 0177,01
.int L141
.byte 020,00;.byte 077,00
.int L142
.byte 0177,00;.byte 020,01
.int L143
.byte 0177,03;.byte 020,00
.int L143
.byte 0177,00;.byte 0124,01
.int L144
.byte 0177,03;.byte 0124,00
.int L144
.byte 0177,00;.byte 024,00
.int L145
.byte 0124,00;.byte 0177,01
.int L146
.byte 0124,03;.byte 0177,00
.int L146
.byte 0124,00;.byte 077,00
.int L147
.byte 0177,00;.byte 0177,01
.int L148
.byte 0177,03;.byte 0177,00
.int L148
.byte 0177,00;.byte 077,00
.int L149
.byte 020,01;.byte 077,04
.int L150
.byte 0124,01;.byte 077,04
.int L151
.byte 020,010;.byte 04,00
.int L152
.byte 0177,010;.byte 04,00
.int L153
.byte 020,010;.byte 020,01
.int L154
.byte 020,010;.byte 0177,01
.int L155
.byte 020,010;.byte 077,00
.int L156
.byte 020,010;.byte 077,04
.int L157
.byte 0124,010;.byte 077,04
.int L158
.byte 020,010;.byte 020,010
.int L159
.byte 020,010;.byte 0177,010
.int L160
.byte 020,010;.byte 077,010
.int L161
.byte 0177,010;.byte 020,01
.int L162
.byte 0177,010;.byte 020,010
.int L163
.byte 0124,010;.byte 077,010
.int L164
.byte 0177,010;.byte 077,00
.int L165
.byte 0177,010;.byte 077,010
.int L166
 # =| and =& ~
.int 0; ci78:
.byte 020,00;.byte 020,01
.int L167
.byte 020,03;.byte 020,00
.int L167
.set L167,move3

.byte 020,00;.byte 077,00
.int L168
.set L168,move5

.byte 0177,00;.byte 020,01
.int L169
.byte 0177,03;.byte 020,00
.int L169
.set L169,move6

.byte 0177,00;.byte 0124,01
.int L170
.byte 0177,03;.byte 0124,00
.int L170
.set L170,move7

.byte 0177,00;.byte 024,00
.int L171
.set L171,move8

.byte 0124,00;.byte 0177,01
.int L172
.byte 0124,03;.byte 0177,00
.int L172
.set L172,move9

.byte 0124,00;.byte 077,00
.int L173
.set L173,move10

.byte 0177,00;.byte 0177,01
.int L174
.byte 0177,03;.byte 0177,00
.int L174
.set L174,move11

.byte 0177,00;.byte 077,00
.int L175
.set L175,move12

.byte 020,010;.byte 010,00
.int L176
.byte 020,010;.byte 020,011
.int L176
.set L176,move13a

.byte 020,010;.byte 020,010
.int L177
.set L177,move13

.byte 020,010;.byte 0177,010
.int L178
.set L178,move14

.byte 020,010;.byte 077,010
.int L179
.set L179,move15

.byte 0177,010;.byte 010,00
.int L180
.set L180,move14a

.byte 0177,010;.byte 020,010
.int L181
.set L181,move16a

.byte 0124,010;.byte 077,010
.int L182
.set L182,move16

.byte 0177,010;.byte 077,010
.int L183
.set L183,move17

 # =^
.int 0; ci79:
.byte 020,010;.byte 077,010
.int L184
.set L184,move15

.byte 0124,010;.byte 077,010
.int L185
.set L185,move16

.byte 0177,010;.byte 077,010
.int L186
 # =+
.int 0; ci70:
.byte 0177,00;.byte 04,00
.int L187
.byte 020,00;.byte 04,00
.int L187
.byte 020,00;.byte 05,00
.int L188
.byte 020,01;.byte 020,01
.int L189
.set L189,move3

.byte 020,01;.byte 0177,01
.int L190
.set L190,move4

.byte 020,01;.byte 077,00
.int L191
.set L191,move5

.byte 0177,00;.byte 05,00
.int L192
.set L192,move2

.byte 0124,01;.byte 0177,01
.int L193
.set L193,move9

.byte 020,00;.byte 0177,01
.int L194
.byte 020,00;.byte 077,00
.int L195
.byte 0124,01;.byte 077,00
.int L196
.set L196,move10

.byte 0177,01;.byte 077,00
.int L197
.set L197,move12

.byte 0177,00;.byte 077,00
.int L198
.byte 020,010;.byte 010,00
.int L199
.byte 020,010;.byte 020,011
.int L199
.set L199,move13a

.byte 020,010;.byte 020,010
.int L200
.set L200,move13

.byte 020,010;.byte 0177,010
.int L201
.set L201,move14

.byte 020,010;.byte 077,010
.int L202
.set L202,move15

.byte 0177,010;.byte 010,00
.int L203
.byte 0177,010;.byte 020,011
.int L203
.set L203,move14a

.byte 0177,010;.byte 020,010
.int L204
.set L204,move16a

.byte 0124,010;.byte 077,010
.int L205
.set L205,move16

.byte 0177,010;.byte 077,010
.int L206
.set L206,move17

 # =>> (all harder cases handled by =<< -)
.int 0; ci75:
.byte 020,00;.byte 05,00
.int L207
.byte 0177,00;.byte 05,00
.int L208
 # =<<
.int 0; ci76:
.byte 020,00;.byte 05,00
.int L209
.byte 0177,00;.byte 05,00
.int L210
.byte 011,00;.byte 020,01
.int L211
.byte 011,00;.byte 0177,01
.int L212
.byte 011,00;.byte 077,00
.int L213
 # =<< for longs
.int 0; cr92:
.byte 020,010;.byte 020,01
.int L214
.byte 020,010;.byte 077,00
.int L215
.byte 0177,010;.byte 077,00
.int L216
 # field = ...
.int 0; ci16:
.byte 020,00;.byte 020,00
.int L217
.byte 020,00;.byte 077,00
.int L218
.byte 0177,00;.byte 020,00
.int L219
.byte 0124,00;.byte 077,00
.int L220
.byte 0177,00;.byte 024,00
.int L221
.byte 0177,00;.byte 077,00
.int L222
 #
 # c code tables-- set condition codes
 #

.globl	_cctab

.data
_cctab:
.int	106,	cc60
.int	28,	rest
.int	55,	rest
.int	34,	rest
.int	35,	rest
.int	36,	rest
.int	37,	rest
.int	40,	rest
.int	41,	rest
.int	42,	rest
.int	43,	rest
 # & as in "if ((a&b)==0)"
.int	81,	cc81	
.int	48,	rest
.int	60,	cc60
.int	61,	cc60
.int	62,	cc60
.int	63,	cc60
.int	64,	cc60
.int	65,	cc60
.int	66,	cc60
.int	67,	cc60
.int	68,	cc60
.int	69,	cc60
.int	72,	rest
.int	73,	rest
.int	79,	rest
.int	0
.text

 # relationals
.int 0; cc60:
.byte 020,00;.byte 04,00
.int L223
.byte 020,05;.byte 04,04
.int L223
.set L223,move1

.byte 020,04;.byte 04,00
.int L224
.byte 0177,00;.byte 04,00
.int L225
.byte 0177,05;.byte 04,04
.int L225
.set L225,move2

.byte 0177,04;.byte 04,00
.int L226
.byte 077,00;.byte 04,00
.int L227
.byte 077,04;.byte 04,04
.int L227
.byte 020,01;.byte 020,01
.int L228
.byte 020,03;.byte 020,03
.int L228
.set L228,move3

.byte 0177,01;.byte 020,01
.int L229
.byte 0177,03;.byte 020,03
.int L229
.set L229,move6

.byte 077,00;.byte 020,01
.int L230
.byte 077,04;.byte 020,05
.int L230
.set L230,Add1

.byte 0177,01;.byte 0124,01
.int L231
.byte 0177,03;.byte 0124,03
.int L231
.set L231,move7

.byte 0177,01;.byte 024,00
.int L232
.set L232,move8

.byte 077,00;.byte 0124,01
.int L233
.byte 077,04;.byte 0124,05
.int L233
.set L233,Add2

.byte 077,00;.byte 024,00
.int L234
.byte 077,04;.byte 024,04
.int L234
.set L234,Add3

.byte 0177,01;.byte 0177,01
.int L235
.byte 0177,03;.byte 0177,03
.int L235
.set L235,move11

.byte 0177,01;.byte 077,00
.int L236
.set L236,move12

.byte 077,00;.byte 077,00
.int L237
.byte 077,04;.byte 077,04
.int L237
.set L237,Add5

.byte 020,010;.byte 04,00
.int L238
.byte 020,010;.byte 010,00
.int L239
.byte 020,010;.byte 020,011
.int L239
.byte 020,010;.byte 020,010
.int L240
.byte 0177,010;.byte 04,00
.int L241
.byte 0177,010;.byte 010,00
.int L242
.byte 0177,010;.byte 020,011
.int L242
.byte 0177,010;.byte 020,010
.int L243
.byte 077,010;.byte 04,00
.int L244
.byte 077,010;.byte 010,00
.int L245
.byte 077,010;.byte 020,011
.int L245
.byte 077,010;.byte 020,010
.int L246
.byte 0177,010;.byte 0124,010
.int L247
.byte 077,010;.byte 0124,010
.int L248
.byte 077,010;.byte 077,010
.int L249
 # & as in "if ((a&b) ==0)"
.int 0; cc81:
.byte 020,00;.byte 020,00
.int L250
.set L250,move3

.byte 0177,00;.byte 020,00
.int L251
.set L251,move6

.byte 077,00;.byte 020,00
.int L252
.set L252,Add1

.byte 077,00;.byte 024,00
.int L253
.set L253,Add3

.byte 077,00;.byte 077,00
.int L254
.set L254,Add5

.byte 020,010;.byte 010,00
.int L255
.byte 020,010;.byte 020,011
.int L255
.byte 0177,010;.byte 010,00
.int L256
.byte 0177,010;.byte 020,011
.int L256
.byte 020,010;.byte 020,010
.int L257
.set L257,lcmp1

.byte 0177,010;.byte 020,010
.int L258
.set L258,lcmp2

.byte 077,010;.byte 020,010
.int L259
.set L259,lcmp3

.byte 0177,010;.byte 0124,010
.int L260
.set L260,lcmp4

.byte 077,010;.byte 0124,010
.int L261
.set L261,lcmp5

.byte 077,010;.byte 077,010
.int L262
.set L262,lcmp6

.byte 077,010;.byte 010,00
.int L263
.byte 077,010;.byte 020,011
.int L263
 # set codes right
.int 0; rest:
.byte 077,00;.byte 077,00
.int L264
.byte 077,04;.byte 077,04
.int L264
 #
 # c code tables-- expression to -(sp)
 #

.globl	_sptab

.data
_sptab:
.int	106,	cs106
.int	40,	cs40
.int	41,	cs40
.int	55,	cs40
.int	48,	cs40
.int	58,	cs58
.int	56,	cs56
.int	0
.text

 # name
.int 0; cs106:
.byte 04,00;.byte 077,00
.int L265
.byte 04,04;.byte 077,00
.int L265
.byte 020,01;.byte 077,00
.int L266
.byte 0177,01;.byte 077,00
.int L267
.byte 020,010;.byte 077,00
.int L268
 # +, -, |, &~
.int 0; cs40:
.byte 020,00;.byte 05,00
.int L269
.byte 020,00;.byte 020,01
.int L270
.byte 020,00;.byte 0177,01
.int L271
.byte 020,00;.byte 077,00
.int L272
 # integer to long
.int 0; cs58:
.byte 077,011;.byte 077,00
.int L273
.byte 077,00;.byte 077,00
.int L274
 # float to long
.int 0; cs56:
.byte 077,04;.byte 077,00
.int L275
 # setup for structure assign
.int 0; ci116:
.byte 077,00;.byte 024,00
.int L276
.byte 077,00;.byte 077,00
.int L277
.int 0
