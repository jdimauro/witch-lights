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
function Convert-ExcelToArduino
{
    [CmdletBinding()]
    [Alias()]
    Param
    (
        # Specifies path to excel file
        [Parameter(ValueFromPipelineByPropertyName=$true,
                   Position=0)]
        [alias('Excel','File')]
        $PathToFile = "C:\Users\jdimauro\Documents\GitHub\witch-lights\animations\1-classic-scanner.xlsx",

        # Param2 help description
        [int]
        $Param2
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
        $animationValues = Foreach ($animationFrame in $animationImport)
        {
            # add strcpy and strcat to beginnings of strings

            # 

            # loop through each property of the animation frame, starting at row 2 (the first row with animation data)
            for ($i = 2; $i -le $AnimationFrameWidth; $i++)
            {
                $property = "P$i"
                $animationFrame.$property

            }
        }
        $animationValues
    }
    End
    {
        #
    }
}

Convert-ExcelToArduino

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
