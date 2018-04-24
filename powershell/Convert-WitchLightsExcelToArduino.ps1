<#
.Synopsis
   Converts excel file with numbers in cells to arduino code for the Witch Lights animation.
.DESCRIPTION
   Long description
.EXAMPLE
   Example of how to use this cmdlet
.EXAMPLE
   Another example of how to use this cmdlet
#>
Param
(
    # Specifies path to excel file
    [Parameter(ValueFromPipelineByPropertyName=$true,
                Position=0,
                Mandatory=$true)]
    [alias('Excel','File')]
    $PathToFile,

    # Specifies a name for the animation pattern generated
    [Parameter(Mandatory=$true)]
    [string]$animationName
)

function Convert-ExcelToArduino
{
    [CmdletBinding()]
    [Alias()]
    Param
    (
        # Specifies path to excel file
        [Parameter(ValueFromPipelineByPropertyName=$true,
                   Position=0,
                   Mandatory=$true)]
        [alias('Excel','File')]
        $PathToFile,

        # Specifies a name for the animation pattern generated
        [Parameter(Mandatory=$true)]
        [string]$animationName
    )

    Begin
    {
        # Import excel file and store it as a variable
        $animationImport = Import-Excel $PathToFile -NoHeader -ErrorAction Stop
        # $animationImport | gm

        # Count the entries in $animationImport[0] to get the frame width
        $AnimationFrameWidth = $animationImport[0].psobject.Properties.length | Measure-Object | select -ExpandProperty Count

        # Count the rows in $animationImport to get the frame count
        $AnimationFrameCount = $animationImport.length
        
    }
    Process
    {
        # Loop through the rows of $animationImport and for each row, use $animationImport[3].$property by looping from 1 to
        # $AnimationFrameWidth and making $property = "P$i"

        # Array to hold a list of animation frame arrays
        [array]$animationArray

        Foreach ($animationFrame in $animationImport)
        {
            # Array to hold each animation frame
            $animationFrameArray = New-Object System.Collections.Generic.List[System.Object]

            # loop through each property of the animation frame
            for ($i = 1; $i -le $AnimationFrameWidth; $i++)
            {

                $property = "P$i"

                # Use an if statement to check if the property is blank. If so, return a space " "

                if ($animationFrame.$property -eq $null)
                {
                    # Write-Host "`'$($animationFrame.$property)`'"
                    $animationFrameArray.Add(" ")
                    # pause
                } 
                else 
                {
                    $animationFrameArray.Add($animationFrame.$property.tostring())
                }
            }

            # Check that the return array does have the correct number of values
            if ($animationFrameArray.count -ne $AnimationFrameWidth)
            {
                Write-Error "`$animationFrameArray should have $animationFrameWidth entries, but instead it has $($animationFrameArray.count)."
                break
            }
            else
            {
                # Append the animation array to the full output array
                $animationArray += , $animationFrameArray
            }

            # Debug entries
            # $animationArray.count
            # pause
            # $animationArray
            # pause
        }

        # $animationArray

        if ($animationArray.Count -ne $AnimationFrameCount)
        {
            Write-Error "`$animationArray should contain $AnimationFrameCount entries, but instead contains $($animationArray.Count) entries"
            break
        }

        [array]$animationOutputArray
        
        Foreach ($array in $animationArray)
        {
            [string]$animationOutputFrameString = $array -join ""
            $animationOutputArray += , $animationOutputFrameString
        }

        # $animationOutputArray
        # pause

        $animationOutputArrayIndex = 0
        
        [array]$animationArduinoOutputArray

        Foreach ($string in $animationOutputArray)
        {
            if ($animationOutputArrayIndex -eq 0)
            {
                $stringSpacePrepend = (" " * $animationName.Length)

                # Prepend "//                     " to string
                $string = "//                 " + $stringSpacePrepend + $string

                $animationArduinoOutputArray += , $string.tostring()

                $animationOutputArrayIndex++
            } 
            elseif ($animationOutputArrayIndex -eq 1)
            {
                # Prepend '     strcpy(afc_w8v1, "' to string
                # Append  '");' to string
                $string = "     strcpy(afc_$animationName, `"" + $string + "`");"

                $animationArduinoOutputArray += , $string.tostring()
                $animationOutputArrayIndex++
            }
            else
            {
                $string = "     strcat(afc_$animationName, `"" + $string + "`");"
                $animationArduinoOutputArray += , $string.tostring()
            }

        }
        $animationArduinoOutputArray

    }
    End
    {
        # 
    }
}

Convert-ExcelToArduino $PathToFile -animationName $animationName

<#
    Scanner animation example:

                      12345678901234567890123    # first line of excel sheet
    strcpy(afc_w8v1, "          123456788    "); # 1st line strcpy()
    strcat(afc_w8v1, "           12345688    "); # rest of lines strcat()
    strcat(afc_w8v1, "            12348876   "); 
    strcat(afc_w8v1, "             128876 4  "); # animation name 'afc' -- ?? + 'w8' = 8 pixel tail, 'v1' = version
    strcat(afc_w8v1, "              88765  2 "); 
    strcat(afc_w8v1, "             887654   1");
    strcat(afc_w8v1, "            8876543    ");
    strcat(afc_w8v1, "           88765432    ");
    strcat(afc_w8v1, "          887654321    ");
    strcat(afc_w8v1, "         887654321     ");
    strcat(afc_w8v1, "        887654321      ");
    strcat(afc_w8v1, "       887654321       ");
    strcat(afc_w8v1, "      887654321        ");
    strcat(afc_w8v1, "     887654321         ");
    strcat(afc_w8v1, "    887654321          ");
    strcat(afc_w8v1, "    88654321           ");
    strcat(afc_w8v1, "   67884321            ");
    strcat(afc_w8v1, "  4 678821             ");
    strcat(afc_w8v1, " 2  56788              ");
    strcat(afc_w8v1, "1   456788             ");
    strcat(afc_w8v1, "    3456788            ");
    strcat(afc_w8v1, "    23456788           ");

    strcat(afc_w8v1, "    123456788          ");
    strcat(afc_w8v1, "     123456788         ");
    strcat(afc_w8v1, "      123456788        ");
    strcat(afc_w8v1, "       123456788       ");
    strcat(afc_w8v1, "        123456788      ");
    strcat(afc_w8v1, "         123456788     ");


#define $AnimationName_FRAME_WIDTH $AnimationFrameWidth -- calculate length of each row and store as this variable
#define $AnimationName_FRAMES $AnimationFrameCount -- calculate how many rows of the sheet and store as this variable

#>
