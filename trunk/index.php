<HTML>
<HEAD>
<TITLE>Alpha One Labs Lights</TITLE>
<SCRIPT language="JavaScript">
<!--
    function do_update(x)
    {
        // Simply ignore invalid updates
        if (x.readyState != 4) return;
        if (x.status != 200) return;

        var ret = 0;
        var newstate = x.responseXML.getElementsByTagName("decimal");
        if (newstate.length == 0) return;
        ret = newstate[0].firstChild.nodeValue;

        for (var i=0, j=1; i < 6; ++i, j <<= 1)
        {
            document.forms[0].check_[i].checked = ((ret & j) == j);
        }
    }

    function update_state(cmd)
    {
        var x = new XMLHttpRequest();
        var url = "update.php";
        x.onreadystatechange = function() { do_update(x); };
        if (typeof(cmd) != "undefined" && cmd.length)
        {
            url += "?cmd="+cmd;
        }
        x.open("GET",url,true);
        x.send(null);
    }

    function box_checked(box)
    {
        update_state(box);
    }

    update_state();
    window.setInterval("update_state()", 3000);
// -->
</SCRIPT>
</HEAD>
<BODY>
<FORM>
<input type="hidden" name="last_cmd"/>
<TABLE border="0">
<?php 
    print "<TR>\n";
    for ($i=0, $j=1; $i < 6; ++$i, $j <<= 1)
    {
        print "<TD>$i</TD><TD><INPUT type='checkbox' name='check_' onchange='box_checked(\"$i\")'";
        if ($currentstate & $j)
            print " checked='checked'";
        print " />";
        print "</TD>\n";
    }
    print "</TR>\n";
?><TR>
    <TD colspan=3><button id="allon" onclick='box_checked("N")'/>All On</button></TD>
    <TD colspan=3><button id="alloff" onclick='box_checked("F")'/>All Off</button></TD>
    </TR>
</TABLE>
</FORM>
</BODY>
</HTML>
