# update-counter

<pre>
update-counter <em>options</em> <em>counter-file</em>
</pre>

* Increments the value in specified file and prints the value to stdout.
* The file will be created automatically if it does not exist.
* If the file has unrecognized content, it will be reset.
* Using `lockf()` for locking.

## options

<dl>
<dt>-initval <em>value</em>
<dd>

* The initial value for missing or erroneous counter file.
* Default: 0

<dt>-reinitialize
<dd>

* Forces initializing the counter value.

</dl>
