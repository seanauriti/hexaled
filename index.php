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
        var newstate = x.responseXML.getElementsByTagName("decimal"));
        if (newstate.length == 0) return;
        ret = newstate[0].innerHTML;

        for (var i=0, j=1; i < 6; ++i, j <<= 1)
        {
            document.forms[0].check[i].checked = ((ret & j) == j);
        }
    }

    function update_state(cmd)
    {
        var x = new XMLHttpRequest();
        var url = "update.php";
        x.onReadyStateChange = function() { do_update(x); };
        if (cmd && cmd.length)
            url += "?cmd="+cmd;
        x.open("GET",url,true);
    }

    function box_checked(box)
    {
        update_state(box);
    }

    window.setInterval("update_state()", 3000);
// -->
</SCRIPT>
</HEAD>
<BODY>
<FORM>
<input type="hidden" name="last_cmd"/>
<TABLE border="0">
<?php 
    for ($i=0, $j=1; $i < 6; ++$i, $j <<= 1)
    {
        print "<TR>\n";
        print "<TD>$i</TD><TD><INPUT type='checkbox' name='check_' onchange='box_checked($i)'"
        if ($currentstate & $j)
            print " checked='checked'";
        print " />";
        print "</TD>\n";
        print "</TR>\n";
    }
?>
</TABLE>
</FORM>
</BODY>
</HTML>
