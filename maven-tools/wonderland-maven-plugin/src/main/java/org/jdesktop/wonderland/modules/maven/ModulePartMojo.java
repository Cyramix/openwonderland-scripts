package org.jdesktop.wonderland.modules.maven;

/*
 * Copyright 2001-2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.project.MavenProject;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import org.apache.maven.model.Dependency;

/**
 * Goal which touches a timestamp file.
 *
 * @goal modulePart
 * @phase package
 */
public class ModulePartMojo
    extends AbstractMojo
{
    /**
     * Location of the output.
     * @parameter expression="${project.build.directory}"
     * @required
     */
    private File outputDirectory;

    /**
     * Name of the module part
     * @parameter
     * @required
     */
    private String part;

    /**
     * @parameter default-value="${project}"
     * @required
     * @readonly
     */
    private MavenProject project;

    public MavenProject getProject() {
        return project;
    }

    public void execute()
        throws MojoExecutionException
    {
        MavenProject parent = getProject().getParent();
        if (parent == null) {
            throw new MojoExecutionException("Unable to find parent");
        }

        File baseDir = parent.getBasedir();
        getProject().getDependencies();
        Dependency d = new Dependency();
        d.
        getLog().warn("Packaging module");

        File f = outputDirectory;
        f = f.getParentFile();
        f = f.getParentFile();
        f = new File(f, "dist");
        
        if ( !f.exists() )
        {
            f.mkdirs();
        }

        File touch = new File( f, "touch.txt" );

        FileWriter w = null;
        try
        {
            w = new FileWriter( touch );

            w.write( "touch.txt" );
        }
        catch ( IOException e )
        {
            throw new MojoExecutionException( "Error creating file " + touch, e );
        }
        finally
        {
            if ( w != null )
            {
                try
                {
                    w.close();
                }
                catch ( IOException e )
                {
                    // ignore
                }
            }
        }
    }
}
